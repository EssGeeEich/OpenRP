#include "object.h"
#include "drawable.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace LuaApi {
	
// Model
struct MatInfo {
    aiColor3D diffuse;
    aiColor3D specular;
    aiColor3D ambient;
    aiColor3D emissive;
    aiVector3D oss;
    
    struct PerTex {
        std::string tex;
        int uv;
        aiTextureMapMode map;
    };
    PerTex textures[DrawableState::MAX_TEXTURES];
};

GLenum TextureWrapToGL(aiTextureMapMode tmm)
{
    switch(tmm)
    {
    default:
    case aiTextureMapMode_Wrap:
        return GL_REPEAT;
    case aiTextureMapMode_Decal:
    case aiTextureMapMode_Clamp:
        return GL_CLAMP_TO_BORDER;
    case aiTextureMapMode_Mirror:
        return GL_MIRRORED_REPEAT;
    }
}

bool ModelImpl::load(std::string const& path)
{
    // Attribute 0: Position
    // Attribute 1: Normal
    // Attribute 2: Tangent
    // Attribute 3: Bitangent
    // Attribute 4: UV
    // Attribute 5: Color
    // Attribute 6: UV
    // Attribute 7: Color
    // ...
    
    // Uniform 0: Material Color
    // Uniform 1: Material Specular
    // Uniform 2: Material Ambient
    // Uniform 3: Material Emissive
    // Uniform 4: Material Opacity, Shininess, Shininess Strength (XYZ)
    
    //  0 - Uniform  8: Texture Diffuse
    //  0 - Uniform  9: Diffuse UV
    //  1 - Uniform 10: Texture Specular
    //  1 - Uniform 11: Specular UV
    //  2 - Uniform 12: Texture Ambient
    //  2 - Uniform 13: Ambient UV
    //  3 - Uniform 14: Texture Emissive
    //  3 - Uniform 15: Emissive UV
    //  4 - Uniform 16: Texture Normals
    //  4 - Uniform 17: Normals UV
    //  5 - Uniform 18: Texture Heights
    //  5 - Uniform 19: Heights UV
    //  6 - Uniform 20: Texture Opacity
    //  6 - Uniform 21: Opacity UV
    //  7 - Uniform 22: Texture Shininess
    //  7 - Uniform 23: Shininess UV
    //  8 - Uniform 24: Texture Displacement
    //  8 - Uniform 25: Displacement UV
    //  9 - Uniform 26: Texture Lightmap
    //  9 - Uniform 27: Lightmap UV
    // 10 - Uniform 28: Texture Reflection
    // 10 - Uniform 29: Reflection UV
    
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path, 
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);
    if(!scene)
        return false;
    
    m_bones.clear();
    m_bones.reserve(scene->mNumMeshes);
    
    // Load the model
    for(std::size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        
        m_bones.emplace_back();
        ModelBone& objectBone = m_bones.back();
        objectBone.Init();
        
        ModelStorage& currentModel = objectBone->m_model;
        currentModel.Init();
        if(!currentModel->create_indexed(mesh->mNumVertices))
            return false;
        currentModel->bind();
        
        Lua::Array<float> pos;
        Lua::Array<float> normal;
        Lua::Array<float> tangent;
        Lua::Array<float> bitangent;
        std::uint32_t uvComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
        Lua::Array<float> uv[AI_MAX_NUMBER_OF_TEXTURECOORDS];
        Lua::Array<float> color[AI_MAX_NUMBER_OF_COLOR_SETS];
        
        Lua::Array<std::uint16_t> ix16;
        Lua::Array<std::uint32_t> ix32;
        
        // Allocate buffers
        {
            if(mesh->HasPositions())
            {
                pos.m_data.reserve(mesh->mNumVertices);
            }
            
            if(mesh->HasNormals())
            {
                normal.m_data.reserve(mesh->mNumVertices);
            }
            
            if(mesh->HasTangentsAndBitangents())
            {
                tangent.m_data.reserve(mesh->mNumVertices);
                bitangent.m_data.reserve(mesh->mNumVertices);
            }
            
            for(std::size_t i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
            {
                if(mesh->HasTextureCoords(i))
                {
                    uv[i].m_data.reserve(mesh->mNumVertices);
                    uvComponents[i] = mesh->mNumUVComponents[i];
                }
                else
                    uvComponents[i] = 0;
            }
            
            for(std::size_t i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i)
            {
                if(mesh->HasVertexColors(i))
                {
                    color[i].m_data.reserve(mesh->mNumVertices);
                }
            }
        }
        
        // Initialize Index Buffer
        {
            std::size_t IndexCount = 0;
            for(std::size_t j = 0; j < mesh->mNumFaces; ++j)
            {
                IndexCount += mesh->mFaces[j].mNumIndices;
            }
            
            if(mesh->mNumVertices > 0xFFFE)
            {
                ix32.m_data.reserve(IndexCount);
                
                for(std::size_t j = 0; j < mesh->mNumFaces; ++j)
                {
                    aiFace const& face = mesh->mFaces[j];
                    if(face.mNumIndices != 3)
                        return false;
                    for(std::size_t k = 0; k < 3; ++k) // face.mNumIndices
                    {
                        ix32.m_data.push_back(face.mIndices[k]);
                    }
                }
            }
            else
            {
                ix16.m_data.reserve(IndexCount);
                
                for(std::size_t j = 0; j < mesh->mNumFaces; ++j)
                {
                    aiFace const& face = mesh->mFaces[j];
                    if(face.mNumIndices != 3)
                        return false;
                    for(std::size_t k = 0; k < 3; ++k) // face.mNumIndices
                    {
                        ix16.m_data.push_back(face.mIndices[k]);
                    }
                }
            }
        }
        
        // Parse the meshes
        for(std::size_t i = 0; i < mesh->mNumVertices; ++i)
        {            
#define SET1(array, var) {array.m_data.push_back(var.x);}
#define SET2(array, var) {array.m_data.push_back(var.x); array.m_data.push_back(var.y);}
#define SET3(array, var) {array.m_data.push_back(var.x); array.m_data.push_back(var.y); array.m_data.push_back(var.z);}
#define SETCOL(array, var) {array.m_data.push_back(var.r); array.m_data.push_back(var.g); array.m_data.push_back(var.b); array.m_data.push_back(var.a);}
            
            objectBone->m_name = mesh->mName.C_Str();
            if(mesh->HasPositions())
            {
                aiVector3D vPos = mesh->mVertices[i];
                SET3(pos, vPos);
            }
            
            if(mesh->HasNormals())
            {
                aiVector3D vNormal = mesh->mNormals[i];
                SET3(normal, vNormal);
            }
            
            if(mesh->HasTangentsAndBitangents())
            {
                aiVector3D vTangent = mesh->mTangents[i];
                aiVector3D vBitangent = mesh->mBitangents[i];
                SET3(tangent, vTangent);
                SET3(bitangent, vBitangent);
            }
            
            
            for(std::size_t j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j)
            {
                if(mesh->HasTextureCoords(j))
                {
                    aiVector3D vUV = mesh->mTextureCoords[j][i];
                    
                    switch(uvComponents[j])
                    {
                    case 1:
                        SET1(uv[j], vUV);
                        break;
                    case 2:
                        SET2(uv[j], vUV);
                        break;
                    case 3:
                        SET3(uv[j], vUV);
                        break;
                    default:
                        uvComponents[j] = 0;
                    }
                }
            }
            for(std::size_t j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; ++j)
            {
                if(mesh->HasVertexColors(j))
                {
                    aiColor4D col = mesh->mColors[j][i];
                    
                    SETCOL(color[j], col);
                }
            }
        }
        
        // Set the index buffers
        {
            if(ix16.m_data.size())
            {
                if(!currentModel->setindices(ix16))
                    return false;
            }
            else
            {
                if(!currentModel->setindices_32(ix32))
                    return false;
            }
        }
        
        // Set the other buffers
        {
            if(mesh->HasPositions() && !currentModel->set3d(0,pos))
                return false;
            if(mesh->HasNormals() && !currentModel->set3d(1,normal))
                return false;
            if(mesh->HasTangentsAndBitangents() &&
                    (!currentModel->set3d(2,tangent) ||
                     !currentModel->set3d(3,bitangent)))
                return false;
            
            for(std::size_t i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
            {
                if(!mesh->HasTextureCoords(i))
                    continue;
                switch(uvComponents[i])
                {
                case 1:
                    if(!currentModel->set1d(4 + (i*2),uv[i]))
                        return false;
                    break;
                case 2:
                    if(!currentModel->set2d(4 + (i*2),uv[i]))
                        return false;
                    break;
                case 3:
                    if(!currentModel->set3d(4 + (i*2),uv[i]))
                        return false;
                    break;
                }
            }
            
            for(std::size_t i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i)
            {
                if(!mesh->HasVertexColors(i))
                    continue;
                if(!currentModel->set4d(5 + (i*2),color[i]))
                    return false;
            }
        }
        
        // Send the data to OpenGL
        if(!currentModel->lock())
            return false;
    }
    
    std::vector<MatInfo> materials;
    materials.reserve(scene->mNumMaterials);
    
    // Inspect and pack the materials
    for(std::size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        materials.emplace_back();
        MatInfo& dstMat = materials.back();
        
        aiMaterial* material = scene->mMaterials[i];
        aiColor3D color;
        float f;
        if(material->Get(AI_MATKEY_COLOR_DIFFUSE,color) == AI_SUCCESS)
            dstMat.diffuse = color;
        if(material->Get(AI_MATKEY_COLOR_SPECULAR,color) == AI_SUCCESS)
            dstMat.specular = color;
        if(material->Get(AI_MATKEY_COLOR_AMBIENT,color) == AI_SUCCESS)
            dstMat.ambient = color;
        if(material->Get(AI_MATKEY_COLOR_EMISSIVE,color) == AI_SUCCESS)
            dstMat.emissive = color;
        if(material->Get(AI_MATKEY_OPACITY,f) == AI_SUCCESS)
            dstMat.oss.x = f;
        if(material->Get(AI_MATKEY_SHININESS,f) == AI_SUCCESS)
            dstMat.oss.y = f;
        if(material->Get(AI_MATKEY_SHININESS_STRENGTH,f) == AI_SUCCESS)
            dstMat.oss.z = f;
        
        aiString str;
        unsigned int uv = 0;
        aiTextureMapMode tmm = aiTextureMapMode_Wrap;
        
#define MAP_ASSIMP_MAT(assimp_mat, tex_unit)\
        if(material->GetTexture(assimp_mat,0,&str,nullptr,&uv,nullptr,nullptr,&tmm) == AI_SUCCESS) {\
            dstMat.textures[tex_unit].tex = str.C_Str();\
            dstMat.textures[tex_unit].uv = uv;\
            dstMat.textures[tex_unit].map = tmm;\
        }
        
        MAP_ASSIMP_MAT(aiTextureType_DIFFUSE, 0);
        MAP_ASSIMP_MAT(aiTextureType_SPECULAR, 1);
        MAP_ASSIMP_MAT(aiTextureType_AMBIENT, 2);
        MAP_ASSIMP_MAT(aiTextureType_EMISSIVE, 3);
        MAP_ASSIMP_MAT(aiTextureType_NORMALS, 4);
        MAP_ASSIMP_MAT(aiTextureType_HEIGHT, 5);
        MAP_ASSIMP_MAT(aiTextureType_OPACITY, 6);
        MAP_ASSIMP_MAT(aiTextureType_SHININESS, 7);
        MAP_ASSIMP_MAT(aiTextureType_DISPLACEMENT, 8);
        MAP_ASSIMP_MAT(aiTextureType_LIGHTMAP, 9);
        MAP_ASSIMP_MAT(aiTextureType_REFLECTION, 10);
    }
    
    // Apply the materials
    for(std::size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        ModelBone& bone = m_bones[i];
        if(mesh->mMaterialIndex < materials.size())
        {
            MatInfo& mat = materials[mesh->mMaterialIndex];
            
            Lua::Arg<bool> bTrue = Lua::CopyToArg<bool>(true);
#define MAP3(propname, var) bone->Material()->Set ## propname(var.r, var.g, var.b)
#define MAP1(propname, var) bone->Material()->Set ## propname(var)
#define MAPTX(propname, id)\
            {\
                if(!mat.textures[id].tex.empty())\
                {\
                    Texture t;\
                    t.Init();\
                    if(t->load(mat.textures[id].tex, bTrue))\
                    {\
                        t->setwraps(TextureWrapToGL(mat.textures[id].map));\
                        bone->Material()->Set ## propname ## Texture(std::move(t));\
                        bone->Material()->Set ## propname ## UV(mat.textures[id].uv);\
                    }\
                }\
            }
            
            MAP3(DiffuseColor, mat.diffuse);
            MAP3(SpecularColor, mat.specular);
            MAP3(AmbientColor, mat.ambient);
            MAP3(EmissiveColor, mat.emissive);
            MAP1(Opacity, mat.oss.x);
            MAP1(Shininess, mat.oss.y);
            MAP1(ShininessStrength, mat.oss.y);
            
            MAPTX(Diffuse, 0);
            MAPTX(Specular, 1);
            MAPTX(Ambient, 2);
            MAPTX(Emissive, 3);
            MAPTX(Normals, 4);
            MAPTX(Height, 5);
            MAPTX(Opacity, 6);
            MAPTX(Shininess, 7);
            MAPTX(Displacement, 8);
            MAPTX(Lightmap, 9);
            MAPTX(Reflection, 10);
        }
    }
    
    return true;
}

std::size_t ModelImpl::BoneCount() const {
    return m_bones.size();
}
Lua::ReturnValues ModelImpl::GetBoneByName(std::string const& name) {
    for(std::size_t i = 0; i < m_bones.size(); ++i)
    {
        if(m_bones[i]->Name() == name)
            return Lua::Return(m_bones[i]);
    }
    return Lua::Return();
}
Lua::ReturnValues ModelImpl::GetBoneByNumber(std::size_t i) {
    if(i < m_bones.size())
        return Lua::Return(m_bones[i]);
    return Lua::Return();
}

}
