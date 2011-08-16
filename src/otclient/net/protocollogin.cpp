#include "protocollogin.h"
#include <framework/net/outputmessage.h>
#include <framework/net/rsa.h>
#include <framework/luascript/luainterface.h>
#include <boost/bind.hpp>

// TODO just testing
#include "protocolgame.h"
#include <otclient/core/game.h>

ProtocolLogin::ProtocolLogin()
{
    enableChecksum();
}

void ProtocolLogin::login(const std::string& accountName, const std::string& accountPassword)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_accountName = accountName;
    m_accountPassword = accountPassword;

    std::string host = "sv3.radbr.com";
    uint16 port = 7171;

    connect(host, port);
}

void ProtocolLogin::onConnect()
{
    sendLoginPacket();
}

void ProtocolLogin::onRecv(InputMessage& inputMessage)
{
    while(!inputMessage.end()) {
        uint8 opt = inputMessage.getU8();
        switch(opt) {
        case 0x0A:
            parseError(inputMessage);
            break;
        case 0x14:
            parseMOTD(inputMessage);
            break;
        case 0x1e:
            callLuaField("onError", "Client needs update.");
            break;
        case 0x64:
            parseCharacterList(inputMessage);
            break;
        }
    }
    disconnect();
}

void ProtocolLogin::onError(const boost::system::error_code& error)
{
    callLuaField("onError", error.message());
}

void ProtocolLogin::sendLoginPacket()
{
    OutputMessage oMsg;

    oMsg.addU8(0x01); // Protocol id
    oMsg.addU16(0x02);  // OS
    oMsg.addU16(862); // Client version

    oMsg.addU32(0x4E12DAFF); // Data Signature
    oMsg.addU32(0x4E12DB27); // Sprite Signature
    oMsg.addU32(0x4E119CBF); // Picture Signature

    oMsg.addU8(0); // First RSA byte must be 0x00 // 1

    // Add xtea key
    generateXteaKey();
    oMsg.addU32(m_xteaKey[0]); // 5
    oMsg.addU32(m_xteaKey[1]); // 9
    oMsg.addU32(m_xteaKey[2]); // 13
    oMsg.addU32(m_xteaKey[3]); // 17

    oMsg.addString(m_accountName); // Account Name // 19
    oMsg.addString(m_accountPassword); // Password // 21

    // Packet data must have since byte 0, start, 128 bytes
    oMsg.addPaddingBytes(128 - (21 + m_accountName.length() + m_accountPassword.length()));

    // Encrypt msg with RSA
    if(!Rsa::encrypt((char*)oMsg.getBuffer() + 6 + oMsg.getMessageSize() - 128, 128, OTSERV_PUBLIC_RSA))
        return;

    send(oMsg);

    enableXteaEncryption();

    recv();
}

void ProtocolLogin::parseError(InputMessage& inputMessage)
{
    std::string error = inputMessage.getString();
    callLuaField("onError", error);
}

void ProtocolLogin::parseMOTD(InputMessage& inputMessage)
{
    std::string motd = inputMessage.getString();
    callLuaField("onMotd", motd);
}

void ProtocolLogin::parseCharacterList(InputMessage& inputMessage)
{
    uint8 characters = inputMessage.getU8();
    for(int i = 0; i < characters; ++i) {
        std::string name = inputMessage.getString();
        std::string world = inputMessage.getString();
        uint32 ip = inputMessage.getU32();
        uint16 port = inputMessage.getU16();

        // TODO just test
        if(i == 0) {
            g_game.loginWorld(m_accountName, m_accountPassword, ip, port, name);
            break;
        }
    }
    /*uint16 premiumDays =*/ inputMessage.getU16();
}
