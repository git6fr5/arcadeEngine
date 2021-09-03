#include "../engine.h"
#include "physics.h" // I don't like this

#define _USE_MATH_DEFINES
#include <math.h>

// Rendering Modules.
#define SDL_MAIN_HANDLED
#include <SDL.h>

using namespace std;
using namespace Engine;

float k_DefaultLength = 25;

class Engine::Visual::Joint {
    public:
        string name;
        float m_Length;
        float m_Angle;
        Joint* m_Children[4];
        int m_ChildrenCount = 0;
        Engine::Physics::Vector m_Vertices[32];
        SDL_Point m_Points[32];
        int m_VertexCount;

        // Updates the vertices of the joints attached.
        void Update(float deltaTime) {
            m_VertexCount = 0;
            m_VertexCount = UpdateVertices(m_Vertices, &m_VertexCount);
        }

        // Adds a child by constructing a new joint.
        // NOTE: Operates on the assumption that we aren't going to add more than 4 children.
        Joint* AddChild(string _name, float length, float angle) {
            m_Children[m_ChildrenCount] = new Joint{ _name, length, angle };
            m_ChildrenCount++;
            return m_Children[m_ChildrenCount - 1];
        }

        // Adds an already constructed joint as a child.
        // NOTE: Operates on the assumption that we aren't going to add more than 4 children.
        Joint* AddChild(string _name, Joint joint) {
            // Creates a new variable at this address.
            m_Children[m_ChildrenCount] = new Joint{ };
            // Copies the values of the passed in local variable to the appropriate address.
            Joint* newJoint = m_Children[m_ChildrenCount];
            memcpy(newJoint, &joint, sizeof(joint));
            newJoint->name = _name;
            m_ChildrenCount++;
            return m_Children[m_ChildrenCount - 1];
        }

        // Prints the joint hierarchy.
        void PrintHierarchy(int depth = 0) {
            for (int i = 0; i < depth + 1; i++) {
                cout << "-->";
            }
            cout << "  " << name << " : " << to_string(m_Length) << " : " << to_string(m_Angle) << "\n\n";
            for (int i = 0; i < m_ChildrenCount; i++) {
                m_Children[i]->PrintHierarchy(depth + 1);
            }
        }

        // Flattens the joint hierarchy to a string array.
        int FlattenHierarchy(string jointNames[], int* jointCount) {
            jointNames[*jointCount] = name;
            (*jointCount)++;
            for (int i = 0; i < m_ChildrenCount; i++) {
                m_Children[i]->FlattenHierarchy(jointNames, jointCount);
            }
            return *jointCount;
        }

        // Returns an array of SDL points.
        SDL_Point* VerticesToPoints(Engine::Physics::Vector origin) {
            for (int i = 0; i < m_VertexCount; i++) {
                m_Points[i] = { (int)m_Vertices[i].x + (int)origin.x, -(int)m_Vertices[i].y + (int)origin.y };
            }
            return m_Points;
        }

    private:
        // Updates the vertices by recurvisely calling itself on each child joint.
        int UpdateVertices(Engine::Physics::Vector vertices[], int* vertexCount, Engine::Physics::Vector origin = Engine::Physics::Vector{}) {
            Engine::Physics::Vector vertex = GetVertex(origin);
            vertices[*vertexCount] = origin;
            vertices[*vertexCount + 1] = vertex;
            *vertexCount = *vertexCount + 2;
            for (int i = 0; i < m_ChildrenCount; i++) {
                m_Children[i]->UpdateVertices(vertices, vertexCount, vertex);
            }
            return *vertexCount;
        }

        // Gets the vertex of this joint.
        Engine::Physics::Vector GetVertex(Engine::Physics::Vector origin) {
            float angle = m_Angle / 180 * M_PI;
            Engine::Physics::Vector vertex = origin + Engine::Physics::Vector{ m_Length * cos(angle), m_Length * sin(angle) };
            return vertex;
        }

};


class Engine::Visual::Skeleton {
    public:
        // Constructs a default shoulder root joint.
        static Joint Shoulder(bool isMirrored = false) {
            // Flip should
            string suffix = ".l";
            if (isMirrored) { 
                suffix = ".r"; 
            }
            Joint shoulder = Joint{ "shoulder" + suffix, k_DefaultLength, (float)isMirrored * 180 };

            Joint* elbow = shoulder.AddChild("elbow" + suffix, k_DefaultLength, (float)isMirrored * 180 + 90);
            (*elbow).AddChild("wrist" + suffix, k_DefaultLength / 2, (float)isMirrored * 180);

            return shoulder;
        }

        // Constructs a default spine root joint.
        static Joint Spine() {
            Joint spine = Joint{ "spine", k_DefaultLength, 90 };
            Joint* torso = spine.AddChild("torso", k_DefaultLength, 90);
            Joint* neck = torso->AddChild("neck", k_DefaultLength, 90);
            Joint shoulder_l = Skeleton::Shoulder();
            Joint shoulder_r = Skeleton::Shoulder(true);
            neck->AddChild("shoulder_l", shoulder_l);
            neck->AddChild("shoulder_r", shoulder_r);
            neck->AddChild("head", k_DefaultLength, 90);
            return spine;
        }
};

class Engine::Visual::Animator {

};

class Engine::Visual::Animation {

};


class Engine::Visual::Particle {
 

};