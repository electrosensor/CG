#pragma once
#include "Face.h"

/*
 * Model class. An abstract class for all types of geometric data.
 */

class Model
{
public:
	virtual ~Model() {}
    virtual const     glm::mat4x4& GetModelTransformation()                                                              = 0;
    virtual const     glm::mat4x4& GetWorldTransformation()                                                              = 0;
    virtual const     glm::mat4x4& GetNormalTransformation()                                                             = 0;
    virtual void      SetModelTransformation(glm::mat4x4& transformation)                                                = 0;
	virtual void      SetWorldTransformation(glm::mat4x4& transformation)                                                = 0;
	virtual void      SetNormalTransformation(glm::mat4x4& transformation)                                               = 0;
	virtual void      Draw(std::tuple<std::vector<Face> , std::vector<glm::vec3> , std::vector<glm::vec3> >& modelData)  = 0;
    virtual glm::vec3 getCentroid()                                                                                      = 0;

    bool isModelRenderingActive()                               { return m_bShouldRender; }
    void setModelRenderingState(bool bIsRenderingStateActive)   { m_bShouldRender = bIsRenderingStateActive; }
    CUBE& getBordersCube()                                      { return m_cubeLines; }
    

    glm::vec3 m_minCoords;
    glm::vec3 m_maxCoords;


protected:
    bool m_bShouldRender;
    CUBE m_cubeLines;

    void buildBorderCube(CUBE& cubeLines)
    {
        INIT_CUBE_COORDS(m_maxCoords, m_minCoords);
                                                                
        glm::vec3       RBN = { cRight  ,  cBottom  ,  cNear  };  //            _________________________RTF=m_maxCoords(x,y,z)
        glm::vec3       RBF = { cRight  ,  cBottom  ,  cFar   };  //           /LTF___________________  /|
        glm::vec3       LBF = { cLeft   ,  cBottom  ,  cFar   };  //          / / ___________________/ / |
        glm::vec3       LBN = { cLeft   ,  cBottom  ,  cNear  };  //         / / /| |               / /  |
        glm::vec3       RTN = { cRight  ,  cTop     ,  cNear  };  //        / / / | |              / / . |
        glm::vec3       RTF = { cRight  ,  cTop     ,  cFar   };  //       / / /| | |             / / /| |
        glm::vec3       LTF = { cLeft   ,  cTop     ,  cFar   };  //      / / / | | |            / / / | |
        glm::vec3       LTN = { cLeft   ,  cTop     ,  cNear  };  //     / / /  | | |           / / /| | |
                                                                  //    / /_/__________________/ / / | | |
                                                                  //   /LTN_____________________/ /  | | |
        cubeLines.lines[0 ] = {      RBN      ,      RBF      };  //   | ____________________RTN| |  | | |
        cubeLines.lines[1 ] = {      RBN      ,      LBN      };  //   | | |    | | |_________| | |__| | |
        cubeLines.lines[2 ] = {      RBN      ,      RTN      };  //   | | |    | |___________| | |____| |
        cubeLines.lines[3 ] = {      RBF      ,      LBF      };  //   | | |   / /LBF_________| | |_  / /RBF
        cubeLines.lines[4 ] = {      RBF      ,      RTF      };  //   | | |  / / /           | | |/ / /
        cubeLines.lines[5 ] = {      LBF      ,      LBN      };  //   | | | / / /            | | | / /
        cubeLines.lines[6 ] = {      LBF      ,      LTF      };  //   | | |/ / /             | | |/ /
        cubeLines.lines[7 ] = {      LBN      ,      LTN      };  //   | | | / /              | | ' /
        cubeLines.lines[8 ] = {      RTN      ,      LTN      };  //   | | |/_/_______________| |  /
        cubeLines.lines[9 ] = {      RTN      ,      RTF      };  //   | |____________________| | /
        cubeLines.lines[10] = {      RTF      ,      LTF      };  //   |________________________|/
        cubeLines.lines[11] = {      LTF      ,      LTN      };  //   LBN=m_minCoords(u,v,w)   RBN   
    }
};

using PModel = Model*;