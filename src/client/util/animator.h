/*
* Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <client/declarations.h>

#include <framework/core/declarations.h>

enum AnimationPhase : int16
{
    AnimPhaseAutomatic = -1,
    AnimPhaseRandom = 254,
    AnimPhaseAsync = 255,
};

enum AnimationDirection : uint8
{
    AnimDirForward = 0,
    AnimDirBackward = 1
};

class Animator : public stdext::shared_object
{
public:
    void unserialize(int animationPhases, const FileStreamPtr& fin);
    void serialize(const FileStreamPtr& fin) const;
    void setPhase(int phase);
    void resetAnimation();

    int getPhase();

    int getPhaseAt(ticks_t time);
    int getStartPhase() const;
    int getTotalDuration() const;
    int getAnimationPhases() const { return m_animationPhases; }
    int getAverageDuration() const { return getTotalDuration() / getAnimationPhases(); }

    bool isAsync() { return m_async; }
    bool isComplete() { return m_isComplete; }

private:
    int getPingPongPhase();
    int getLoopPhase();
    int getPhaseDuration(int phase) const;

    void calculateSynchronous();

    int m_currentDuration{ 0 };
    int m_animationPhases{ 0 };
    int m_currentLoop{ 0 };
    int m_startPhase{ 0 };
    int m_loopCount{ 0 };
    int m_phase{ 0 };

    bool m_isComplete{ false };
    bool m_async{ false };

    std::vector<std::tuple<int, int>> m_phaseDurations;
    AnimationDirection m_currentDirection{ AnimDirForward };
    ticks_t m_lastPhaseTicks{ 0 };
};

#endif