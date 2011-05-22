/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <prerequisites.h>
#include <core/configs.h>
#include <core/resources.h>

Configs g_configs;

bool Configs::load(const std::string& fileName)
{
    m_fileName = fileName;

    std::stringstream fin;
    if(!g_resources.loadFile(fileName, fin))
        return false;

    try {
        FML::Parser parser(fin, fileName);
        FML::Node* doc = parser.getDocument();

        foreach(FML::Node* node, *doc)
            m_confsMap[node->tag()] = node->value();
    } catch(FML::Exception e) {
        flogError("ERROR: Malformed config file: %s", e.what());
        return false;
    }

    return true;
}

void Configs::save()
{
    if(!m_fileName.empty()) {
        FML::Emitter emitter;
        FML::Node *doc = emitter.createDocument();
        doc->write(m_confsMap);
        g_resources.saveFile(m_fileName, emitter.emitDocument());
    }
}

