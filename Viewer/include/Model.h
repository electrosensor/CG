#pragma once
#include "Defs.h"

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
	virtual void    Draw(std::tuple<std::vector<glm::vec4>, std::vector<glm::vec4>, std::vector<glm::vec4> >& modelData) = 0;

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
                                                                
        glm::vec4 RBN       = { cRight , cBottom , cNear ,  1 };  //            _________________________RTF=m_maxCoords(x,y,z)
        glm::vec4 RBF       = { cRight , cBottom , cFar  ,  1 };  //           /LTF___________________  /|
        glm::vec4 LBF       = { cLeft  , cBottom , cFar  ,  1 };  //          / / ___________________/ / |
        glm::vec4 LBN       = { cLeft  , cBottom , cNear ,  1 };  //         / / /| |               / /  |
        glm::vec4 RTN       = { cRight , cTop    , cNear ,  1 };  //        / / / | |              / / . |
        glm::vec4 RTF       = { cRight , cTop    , cFar  ,  1 };  //       / / /| | |             / / /| |
        glm::vec4 LTF       = { cLeft  , cTop    , cFar  ,  1 };  //      / / / | | |            / / / | |
        glm::vec4 LTN       = { cLeft  , cTop    , cNear ,  1 };  //     / / /  | | |           / / /| | |
                                                                  //    / /_/__________________/ / / | | |
        cubeLines.lines[0 ] = {       RBN     ,      RBF      };  //   /LTN___________________ _/ /  | | |
        cubeLines.lines[1 ] = {       RBN     ,      LBN      };  //   | ____________________RTN| |  | | |
        cubeLines.lines[2 ] = {       RBN     ,      RTN      };  //   | | |    | | |_________| | |__| | |
        cubeLines.lines[3 ] = {       RBF     ,      LBF      };  //   | | |    | |___________| | |____| |
        cubeLines.lines[4 ] = {       RBF     ,      RTF      };  //   | | |   / /LBF_________| | |_  / /RBF
        cubeLines.lines[5 ] = {       LBF     ,      LBN      };  //   | | |  / / /           | | |/ / /
        cubeLines.lines[6 ] = {       LBF     ,      LTF      };  //   | | | / / /            | | | / /
        cubeLines.lines[7 ] = {       LBN     ,      LTN      };  //   | | |/ / /             | | |/ /
        cubeLines.lines[8 ] = {       RTN     ,      LTN      };  //   | | | / /              | | ' /
        cubeLines.lines[9 ] = {       RTN     ,      RTF      };  //   | | |/_/_______________| |  /
        cubeLines.lines[10] = {       RTF     ,      LTF      };  //   | |____________________| | /
        cubeLines.lines[11] = {       LTF     ,      LTN      };  //   |________________________|/
                                                                  //   LBN=m_minCoords(u,v,w)   RBN   
    }


};

using PModel = Model*;
