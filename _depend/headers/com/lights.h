#ifndef LIGHTS_H_INCLUDED
#define LIGHTS_H_INCLUDED
#include <com/macros.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <com/utility.h>

namespace me{
    struct Light{
        glm::vec4 color;
        float strength;
    };

    struct GlobalLight : public Light{};

    struct DotLight : public Light{
        glm::vec3 position;
    };

    struct Material{
        glm::vec4 color;
        float shiness;
    };
}

#endif // LIGHTS_H_INCLUDED
