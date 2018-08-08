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
    virtual const glm::mat4x4& GetModelTransformation() = 0;
    virtual const glm::mat4x4& GetWorldTransformation() = 0;
    virtual const glm::mat4x4& GetNormalTransformation() = 0;
    virtual void SetModelTransformation(glm::mat4x4& transformation) = 0;
	virtual void SetWorldTransformation(glm::mat4x4& transformation) = 0;
	virtual void SetNormalTransformation(glm::mat4x4& transformation) = 0;
	virtual pair<vector<glm::vec3>, pair<vector<glm::vec3>, vector<glm::vec3> > >* Draw() = 0;    

    bool isModelRenderingActive() { return m_bShouldRender; }
    void setModelRenderingState(bool bIsRenderingStateActive) { m_bShouldRender = bIsRenderingStateActive; }
    CUBE_LINES& getBordersCube() { return m_cubeLines; }
    virtual glm::vec3 getCentroid() = 0;

    glm::vec3 m_minCoords;
    glm::vec3 m_maxCoords;

protected:
    bool m_bShouldRender;
    CUBE_LINES m_cubeLines;

    void buildBorderCube(CUBE_LINES& cubeLines)
    {
        INIT_CUBE_COORDS(m_maxCoords, m_minCoords);
                                                                
        glm::vec3      RNB = { cRight , cBottom , cNear };  //            _________________________RFT=m_maxCoords(x,y,z)
        glm::vec3      RFB = { cRight , cBottom , cFar  };  //           /LFT___________________  /|
        glm::vec3      LFB = { cLeft  , cBottom , cFar  };  //          / / ___________________/ / |
        glm::vec3      LNB = { cLeft  , cBottom , cNear };  //         / / /| |               / /  |
        glm::vec3      RNT = { cRight , cTop    , cNear };  //        / / / | |              / / . |
        glm::vec3      RFT = { cRight , cTop    , cFar  };  //       / / /| | |             / / /| |
        glm::vec3      LFT = { cLeft  , cTop    , cFar  };  //      / / / | | |            / / / | |
        glm::vec3      LNT = { cLeft  , cTop    , cNear };  //     / / /  | | |           / / /| | |
                                                            //    / /_/__________________/ / / | | |
        cubeLines.line[0 ] =        { RNB, RFB };           //   /LNT___________________ _/ /  | | |
        cubeLines.line[1 ] =        { RNB, LNB };           //   | ____________________RNT| |  | | |
        cubeLines.line[2 ] =        { RNB, RNT };           //   | | |    | | |_________| | |__| | |
        cubeLines.line[3 ] =        { RFB, LFB };           //   | | |    | |___________| | |____| |
        cubeLines.line[4 ] =        { RFB, RFT };           //   | | |   / /LFB_________| | |_  / /RFB
        cubeLines.line[5 ] =        { LFB, LNB };           //   | | |  / / /           | | |/ / /
        cubeLines.line[6 ] =        { LFB, LFT };           //   | | | / / /            | | | / /
        cubeLines.line[7 ] =        { LNB, LNT };           //   | | |/ / /             | | |/ /
        cubeLines.line[8 ] =        { RNT, LNT };           //   | | | / /              | | ' /
        cubeLines.line[9 ] =        { RNT, RFT };           //   | | |/_/_______________| |  /
        cubeLines.line[10] =        { RFT, LFT };           //   | |____________________| | /
        cubeLines.line[11] =        { LFT, LNT };           //   |________________________|/
                                                            //   LNB=m_minCoords(u,v,w)   RNB   
    }


};

typedef Model* PModel;
