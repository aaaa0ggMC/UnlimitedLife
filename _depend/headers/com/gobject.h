#ifndef GOBJECT_H_INCLUDED
#define GOBJECT_H_INCLUDED
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <com/vbo.h>
#include <com/macros.h>
#include <com/utility.h>

namespace me{
    ///GObjects Changer Defines
    #define ME_GROT 0x00000001
    #define ME_GMOD 0x00000010

    class GObject : public Changer{
    public:
        GObject(float x = 0,float y = 0,float z = 0,bool enableMovement = false);

        void SetPosition(float x,float y,float z = 0);
        void SetPosition(glm::vec3& v);
        glm::vec3 GetPosition();

        void Move(float x,float y,float z = 0);
        void Move(glm::vec3 & v);
        void MoveDirectional(float left,float up,float forward = 0);

        glm::mat4* GetMat();
        virtual void UpdateModelMat();

        void BuildMV(glm::mat4 * m);
        void BuildMV(GObject * g);

        void BindVBO(VBO vvbo,VBO cvbo = VBO(0));
        VBO GetVBO();

        void Update(unsigned int=0);

        void SetMovement(bool = true);

        void SetBindings(unsigned int vb = 0,unsigned int cb = 1);

        void Rotate(float x,float y,float z = 0);
        void SetRotation(float x,float y,float z = 0);

        void Scale(float x,float y,float z);
        void SetScale(float x,float y,float z);

        void UpdateRotationMat();

        bool movement;

        glm::vec3 rotations;
        glm::vec3 position;
        glm::vec3 scale;

        glm::mat4 mvmat;
        glm::mat4 mat;
        glm::mat4 rmat;

        glm::mat4 tempMat;

        glm::vec3 left;
        glm::vec3 forward;
        glm::vec3 up;

        unsigned int vbind;
        unsigned int cbind;
    private:
        friend class Window;
        friend class Model;
        VBO vbo;
        VBO coord;
    };
}

#endif // GOBJECT_H_INCLUDED
