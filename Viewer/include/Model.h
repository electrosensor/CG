#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Defs.h"
using namespace std;

/*
 * Model class. An abstract class for all types of geometric data.
 */
class Model
{
public:
	virtual ~Model() {}
    virtual const   glm::mat4x4& GetModelTransformation()                                 = 0;
    virtual const   glm::mat4x4& GetWorldTransformation()                                 = 0;
    virtual const   glm::mat4x4& GetNormalTransformation()                                = 0;
    virtual void    SetModelTransformation(glm::mat4x4& transformation)                   = 0;
	virtual void    SetWorldTransformation(glm::mat4x4& transformation)                   = 0;
	virtual void    SetNormalTransformation(glm::mat4x4& transformation)                  = 0;
	virtual void Draw(tuple<vector<glm::vec4>, vector<glm::vec4>, vector<glm::vec4> >& modelData) = 0;    

    bool isModelRenderingActive()                               { return m_bShouldRender; }
    void setModelRenderingState(bool bIsRenderingStateActive)   { m_bShouldRender = bIsRenderingStateActive; }
    CUBE& getBordersCube()                                      { return m_cubeLines; }
    virtual glm::vec4 getCentroid() = 0;

    glm::vec4 m_minCoords;
    glm::vec4 m_maxCoords;


protected:
    bool m_bShouldRender;
    CUBE m_cubeLines;

    void buildBorderCube(CUBE& cubeLines)
    {
        INIT_CUBE_COORDS(m_maxCoords, m_minCoords);
                                                                
        glm::vec4 RNB       = { cRight , cBottom , cNear ,  1 };  //            _________________________RFT=m_maxCoords(x,y,z)
        glm::vec4 RFB       = { cRight , cBottom , cFar  ,  1 };  //           /LFT___________________  /|
        glm::vec4 LFB       = { cLeft  , cBottom , cFar  ,  1 };  //          / / ___________________/ / |
        glm::vec4 LNB       = { cLeft  , cBottom , cNear ,  1 };  //         / / /| |               / /  |
        glm::vec4 RNT       = { cRight , cTop    , cNear ,  1 };  //        / / / | |              / / . |
        glm::vec4 RFT       = { cRight , cTop    , cFar  ,  1 };  //       / / /| | |             / / /| |
        glm::vec4 LFT       = { cLeft  , cTop    , cFar  ,  1 };  //      / / / | | |            / / / | |
        glm::vec4 LNT       = { cLeft  , cTop    , cNear ,  1 };  //     / / /  | | |           / / /| | |
                                                                  //    / /_/__________________/ / / | | |
        cubeLines.lines[0 ] =            { RNB, RFB };            //   /LNT___________________ _/ /  | | |
        cubeLines.lines[1 ] =            { RNB, LNB };            //   | ____________________RNT| |  | | |
        cubeLines.lines[2 ] =            { RNB, RNT };            //   | | |    | | |_________| | |__| | |
        cubeLines.lines[3 ] =            { RFB, LFB };            //   | | |    | |___________| | |____| |
        cubeLines.lines[4 ] =            { RFB, RFT };            //   | | |   / /LFB_________| | |_  / /RFB
        cubeLines.lines[5 ] =            { LFB, LNB };            //   | | |  / / /           | | |/ / /
        cubeLines.lines[6 ] =            { LFB, LFT };            //   | | | / / /            | | | / /
        cubeLines.lines[7 ] =            { LNB, LNT };            //   | | |/ / /             | | |/ /
        cubeLines.lines[8 ] =            { RNT, LNT };            //   | | | / /              | | ' /
        cubeLines.lines[9 ] =            { RNT, RFT };            //   | | |/_/_______________| |  /
        cubeLines.lines[10] =            { RFT, LFT };            //   | |____________________| | /
        cubeLines.lines[11] =            { LFT, LNT };            //   |________________________|/
                                                                  //   LNB=m_minCoords(u,v,w)   RNB   
    }


};

using PModel = Model*;
