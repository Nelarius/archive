#include <opengl/Opengl.h>
#include <Window.h>
#include <MouseEvents.h>
#include <ImGuiRenderer.h>
#include <imgui/imgui.h>
#include <json11/json11.hpp>
#include <math/Geometry.h>
#include <utils/Assert.h>
#include <utils/File.h>
#include <utils/Log.h>
#include <utils/Random.h>
#include <GL/glew.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#include <json11/json11.hpp>
#include <tinyobj/tiny_obj_loader.h>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <memory>
#include <limits>

void GLAPIENTRY debugCallback( GLenum source,
                    GLenum type,
                    GLuint id,
                    GLenum severity,
                    GLsizei length,
                    const GLchar* message,
                    const void* userParam) {
    std::string debugTypeStr{};
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        debugTypeStr = "GL_DEBUG_TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        debugTypeStr = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        debugTypeStr = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        debugTypeStr = "GL_DEBUG_TYPE_PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        debugTypeStr = "GL_DEBUG_TYPE_PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        debugTypeStr = "GL_DEBUG_TYPE_OTHER";
        break;
    case GL_DEBUG_TYPE_MARKER:
        debugTypeStr = "GL_DEBUG_TYPE_MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        debugTypeStr = "GL_DEBUG_TYPE_PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        debugTypeStr = "GL_DEBUG_TYPE_POP_GROUP";
        break;
    }
    std::string debugSourceStr{};
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        debugSourceStr = "GL_DEBUG_SOURCE_API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        debugSourceStr = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        debugSourceStr = "GL_DEBUG_SOURCE_SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        debugSourceStr = "GL_DEBUG_SOURCE_THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        debugSourceStr = "GL_DEBUG_SOURCE_APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        debugSourceStr = "GL_DEBUG_SOURCE_OTHER";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_LOW:
    LOG_DEBUG << debugSourceStr << ", " << debugTypeStr << ": " << message;
            break;
    }
}

void initialize(pg::Window& window) {
    pg::WindowSettings settings{};
    window.initialize(settings);
    glDebugMessageCallback(&debugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

int main(int argc, char** argv) {
    pg::Window window;

    initialize(window);
    /***
    *      ____                _______         __           __
    *     / __ \___  ___ ___  / ___/ /    ___ / /  ___ ____/ /__ _______
    *    / /_/ / _ \/ -_) _ \/ (_ / /__  (_-</ _ \/ _ `/ _  / -_) __(_-<
    *    \____/ .__/\__/_//_/\___/____/ /___/_//_/\_,_/\_,_/\__/_/ /___/
    *        /_/
    */
    /*
     Panel shader
     */
    std::vector<std::unique_ptr<pg::Shader>> shaderStages{};
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/panel.vert.glsl"), GL_VERTEX_SHADER));
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/panel.frag.glsl"), GL_FRAGMENT_SHADER));
    pg::Program panelShader{ shaderStages };
    shaderStages.clear();
    /*
     Diffuse shader
     */
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/diffuse.vert.glsl"), GL_VERTEX_SHADER));
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/diffuse.frag.glsl"), GL_FRAGMENT_SHADER));
    pg::Program diffuseShader{shaderStages};
    shaderStages.clear();
    /*
     Voxelizer shader
     */
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/voxelize.vert.glsl"), GL_VERTEX_SHADER));
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/voxelize.frag.glsl"), GL_FRAGMENT_SHADER));
    pg::Program voxelizeShader{ shaderStages };
    shaderStages.clear();
    /*
     Voxel geometry builder
     */
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/geometry.comp.glsl"), GL_COMPUTE_SHADER));
    pg::Program geometryShader{ shaderStages };
    shaderStages.clear();
    /*
     3d texture clear shader
     */
    shaderStages.emplace_back(new pg::Shader(pg::fileToString("shaders/clear.comp.glsl"), GL_COMPUTE_SHADER));
    pg::Program clearShader{ shaderStages };
    shaderStages.clear();

    /***
    *       ____                _   _      _ __
    *      /  _/_ _  ___ ___ __(_) (_)__  (_) /_
    *     _/ //  ' \/ _ `/ // / / / / _ \/ / __/
    *    /___/_/_/_/\_, /\_,_/_/ /_/_//_/_/\__/
    *              /___/
    */
    pg::ImGuiRenderer imguiRenderer{ &window, &panelShader };

    pg::MouseEvents mouse{};
    mouse.setPressCallback(SDL_BUTTON_LEFT, [&imguiRenderer]() -> void {
        imguiRenderer.mouseButtonPressed(SDL_BUTTON_LEFT);
    });
    mouse.setPressCallback(SDL_BUTTON_RIGHT, [&imguiRenderer]() -> void {
        imguiRenderer.mouseButtonPressed(SDL_BUTTON_RIGHT);
    });
    mouse.setReleaseCallback(SDL_BUTTON_LEFT, [&imguiRenderer]() -> void {
        imguiRenderer.mouseButtonReleased(SDL_BUTTON_LEFT);
    });
    mouse.setReleaseCallback(SDL_BUTTON_RIGHT, [&imguiRenderer]() -> void {
        imguiRenderer.mouseButtonReleased(SDL_BUTTON_RIGHT);
    });

    const uint32_t TargetDeltaT = 16u;

    /***
    *      ____                _______     __        ______
    *     / __ \___  ___ ___  / ___/ /    / /  __ __/ _/ _/__ _______
    *    / /_/ / _ \/ -_) _ \/ (_ / /__  / _ \/ // / _/ _/ -_) __(_-<
    *    \____/ .__/\__/_//_/\___/____/ /_.__/\_,_/_//_/ \__/_/ /___/
    *        /_/
    */
    // the dimensions of the voxel buffer
    const int dim = 64;

    std::string inputfile = "bunny.obj";
    std::vector<tinyobj::shape_t>       shapes;
    std::vector<tinyobj::material_t>    materials;
    std::string err;

    bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::exit(1);
    }

    PG_ASSERT(shapes.size() == 1u);
    PG_ASSERT(shapes[0].mesh.indices.size() % 3 == 0u);

    std::vector<float> vertices;
    float minx = std::numeric_limits< float >::max();
    float maxx = std::numeric_limits< float >::min();
    float miny = std::numeric_limits< float >::max();
    float maxy = std::numeric_limits< float >::min();
    float minz = std::numeric_limits< float >::max();
    float maxz = std::numeric_limits< float >::min();
    size_t indexOffset = 0;
    for (size_t n = 0; n < shapes[0].mesh.num_vertices.size(); n++) {
        unsigned int ngon = shapes[0].mesh.num_vertices[n];
        for (size_t f = 0u; f < ngon; f++) {
            unsigned int v = shapes[0].mesh.indices[indexOffset + f];
            vertices.push_back(shapes[0].mesh.positions[3 * v + 0]);
            vertices.push_back(shapes[0].mesh.positions[3 * v + 1]);
            vertices.push_back(shapes[0].mesh.positions[3 * v + 2]);
            vertices.push_back(1.f);

            if (shapes[0].mesh.positions[3 * v + 0] < minx) {
                minx = shapes[0].mesh.positions[3 * v + 0];
            }
            else if (shapes[0].mesh.positions[3 * v + 0] > maxx) {
                maxx = shapes[0].mesh.positions[3 * v + 0];
            }
            if (shapes[0].mesh.positions[3 * v + 1] < miny) {
                miny = shapes[0].mesh.positions[3 * v + 1];
            }
            else if (shapes[0].mesh.positions[3 * v + 1] > maxy) {
                maxy = shapes[0].mesh.positions[3 * v + 1];
            }
            if (shapes[0].mesh.positions[3 * v + 2] < minz) {
                minz = shapes[0].mesh.positions[3 * v + 2];
            }
            else if (shapes[0].mesh.positions[3 * v + 2] > maxz) {
                maxz = shapes[0].mesh.positions[3 * v + 2];
            }
        }
        indexOffset += ngon;
    }

    /*for (std::size_t i = 0; i < 10000u; ++i) {
        vertices.push_back(pg::randf(-10.f, 10.f));
        vertices.push_back(pg::randf(-10.f, 10.f));
        vertices.push_back(pg::randf(-10.f, 10.f));
        vertices.push_back(1.f);
    }*/

    pg::AABox aabb{ pg::Vec3f{minx, miny, minz}, pg::Vec3f{maxx, maxy, maxz} };
    pg::BufferObject geometryBuf{GL_ARRAY_BUFFER};
    geometryBuf.dataStore(
        vertices.size(), sizeof(float), vertices.data(), GL_STATIC_DRAW
    );
    pg::VertexArrayObject geomDiffuseVao{ 0 };
    {
        pg::VertexArrayObjectFactory vaoDispenser{ &geometryBuf, &diffuseShader };
        vaoDispenser.addStandardAttribute(pg::VertexAttribute::Vertex);
        geomDiffuseVao = vaoDispenser.getVao();
    }
    pg::VertexArrayObject geomVoxelizeVao{ 0 };
    {
        pg::VertexArrayObjectFactory vaoDispenser{ &geometryBuf, &voxelizeShader };
        vaoDispenser.addStandardAttribute(pg::VertexAttribute::Vertex);
        geomVoxelizeVao = vaoDispenser.getVao();
    }

    pg::BufferObject voxelBuf{ GL_ARRAY_BUFFER };
    {
        const unsigned numElements = dim*dim*dim;
        float* data = (float*)malloc(4*sizeof(float) * numElements);
        voxelBuf.dataStore(numElements, 4*sizeof(float), data, GL_STATIC_DRAW);
        free(data);
    }
    pg::VertexArrayObject voxelDiffuseVao{ 0 };
    {
        pg::VertexArrayObjectFactory vaoDispenser{ &voxelBuf, &diffuseShader };
        vaoDispenser.addStandardAttribute(pg::VertexAttribute::Vertex);
        voxelDiffuseVao = vaoDispenser.getVao();
    }
    pg::Texture voxelTexBuf{ GL_TEXTURE_BUFFER };
    voxelTexBuf.setStore(GL_RGBA32F, voxelBuf);

    pg::BufferObject counterBuffer{ GL_ATOMIC_COUNTER_BUFFER };
    {
        GLuint counter = 0u;
        counterBuffer.dataStore(1, sizeof(GLuint), &counter, GL_STATIC_DRAW);
    }

    // do voxelization "offline" for now
    GLboolean lastDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lastCull = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    // voxelize!
    GLuint texObj = 0u;
    {
        uint8_t* data = (uint8_t*)malloc(dim*dim*dim);
        //memset(data, 1u, dim*dim*dim);
        for (int i = 0; i < dim*dim*dim; i++) {
            data[i] = 0u;
        }
        glGenTextures(1, &texObj);
        glBindTexture(GL_TEXTURE_3D, texObj);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // target, level, internalFormat, width, height, depth, border, format, type, data pointer
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, dim, dim, dim, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
        free(data);
    }

    //// unit, texture name, level, layered, layer, access, format
    //// this binds a level of a texture to an image unit
    glBindImageTexture(1, texObj, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

    // project along the z axis
    pg::Matrix4f xyOrtho = pg::Matrix4f::orthographic(
        aabb.max.x - aabb.min.x,
        aabb.max.y - aabb.min.y,
        0.01f, aabb.max.z - aabb.min.z
    );
    pg::Vec3f center = aabb.center();
    pg::Matrix4f view = pg::Matrix4f::translation(pg::Vec3f{ center.x, center.y, aabb.min.z + 0.01f });
    pg::Matrix4f camera = xyOrtho * view.inverse();

    voxelizeShader.use();
    geomVoxelizeVao.bind();

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer.object());
    {
        pg::Matrix4f model{};
        voxelizeShader.setUniform("model", model);
        voxelizeShader.setUniform("ortho", camera);
        glUniform1i(1, dim);
        glUniform3fv(2, 1, aabb.min.data);
        glUniform3fv(3, 1, aabb.max.data);
    }

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, geometryBuf.count() / geomVoxelizeVao.elementsPerIndex());
    {
        GLuint* counter = (GLuint*)counterBuffer.mapBufferRange(0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
        LOG_DEBUG << "There are " << *counter << " voxels";
        *counter = 0u;
        counterBuffer.unmapBuffer();
    }
    geomVoxelizeVao.unbind();
    voxelizeShader.stopUsing();

    glBindTexture(GL_TEXTURE_3D, 0);

    if (lastDepthTest) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    if (lastCull) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    /***
    *                  _        __
    *      __ _  ___ _(_)__    / /__  ___  ___
    *     /  ' \/ _ `/ / _ \  / / _ \/ _ \/ _ \
    *    /_/_/_/\_,_/_/_//_/ /_/\___/\___/ .__/
    *                                   /_/
    */
    bool running = true;
    uint32_t tdelta{TargetDeltaT};   // 16 milliseconds
    bool showImgui = false;
    while (running) {
        uint32_t start = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        mouse.update(); // does it's own event pump here
        /*
         Imgui code goes here
         */
        imguiRenderer.newFrame(float(tdelta) / 1000.f, mouse.coords().x, mouse.coords().y);
        if (showImgui) {
            {
                ImGui::Begin("Testing");
                ImGui::Text("Hello, world!");
                ImGui::End();
            }
        }

        /*
         GPU stuff goes here
         */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pg::Matrix4f model{};
        float aspect = float(window.width()) / window.height();
        pg::Matrix4f cameraPos = pg::Matrix4f::translation(pg::Vec3f{ 0.f, 0.1f, 0.2f });
        pg::Matrix4f cameraPers = pg::Matrix4f::perspective(1.22f, aspect, 0.1f, 100.f);

        // compute the voxel geometry
        int numPoints = 0;
        {
            const unsigned localGroupSize = 4u;
            const unsigned localGroups = dim / localGroupSize;
            geometryShader.use();
            glBindTexture(GL_TEXTURE_3D, texObj);
            glBindImageTexture(1, texObj, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R8UI);
            glBindImageTexture(2, voxelTexBuf.object(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer.object());
            glUniform3fv(3, 1, aabb.min.data);
            glUniform3fv(4, 1, aabb.max.data);
            glUniform1i(5, dim);
            glDispatchCompute(localGroups, localGroups, localGroups);
            glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
            {
                GLuint* counter = (GLuint*)counterBuffer.mapBufferRange(0, 4, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
                numPoints = counter[0];
                counter[0] = 0u;
                counterBuffer.unmapBuffer();
            }
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            geometryShader.stopUsing();
            // the points should now be in voxelBuf
        }
        // render geometry
        {
            diffuseShader.use();
            diffuseShader.setUniform("model", model);
            pg::Matrix4f camera = cameraPers * cameraPos.inverse();
            diffuseShader.setUniform("camera", camera);
            diffuseShader.setUniform("surfColor", pg::Vec3f(1.f, 1.f, 1.f));
            /*geomDiffuseVao.bind();
            glDrawArrays(GL_TRIANGLES, 0, geometryBuf.count() / geomDiffuseVao.elementsPerIndex());
            geomDiffuseVao.unbind();*/
            voxelDiffuseVao.bind();
            glDrawArrays(GL_POINTS, 0, dim*dim*dim);
            voxelDiffuseVao.unbind();
            diffuseShader.stopUsing();
        }

        {
            panelShader.use();
            imguiRenderer.render();
            panelShader.stopUsing();
        }
        
        window.display();

        /*
        * Sleep for the remainder of the frame, if we have time for it
        * */
        uint32_t end = SDL_GetTicks();
        tdelta = end - start;
        if (tdelta < TargetDeltaT) {
            tdelta = TargetDeltaT;
            SDL_Delay(TargetDeltaT - tdelta);
        }
    }

    return 0;
}
