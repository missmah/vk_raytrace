/*
* Copyright (c) 2014-2020, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include <string>

#include "vulkan/vulkan.hpp"

#include "nvh/gltfscene.hpp"
#include "nvvk/allocator_vk.hpp"
#include "nvvk/debug_util_vk.hpp"
#include "nvvk/descriptorsets_vk.hpp"
#include "structures.h"


/*
  - Loading and storing the glTF scene
  - Creates the buffers and descriptor set for the scene
*/
class Scene
{
public:
  enum EBuffer
  {
    eCameraMat,
    eVertex,
    eIndex,
    eNormal,
    eTexCoord,
    eTangent,
    eColor,
    eMaterial,
    eMatrix,
    ePrimLookup,
    eLights,
    eLast_elem
  };

public:
  void setup(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, uint32_t familyIndex, nvvk::Allocator* allocator)
  {
    m_device           = device;
    m_pAlloc           = allocator;
    m_queueFamilyIndex = familyIndex;
    m_debug.setup(device);
  }

  bool load(const std::string& filename);
  void destroy();
  void updateCamera(const vk::CommandBuffer& cmdBuf, float aspectRatio);


  vk::DescriptorSetLayout getDescLayout() { return m_descSetLayout; }
  vk::DescriptorSet       getDescSet() { return m_descSet; }
  nvh::GltfScene&         getScene() { return m_gltfScene; }
  nvh::GltfStats&         getStat() { return m_stats; }
  nvvk::Buffer&           getBuffer(EBuffer b) { return m_buffers[b]; }
  const std::string&      getSceneName() const { return m_sceneName; }
  SceneCamera&            getCamera() { return m_camera; }

private:
  nvh::GltfScene m_gltfScene;
  nvh::GltfStats m_stats;

  SceneCamera m_camera{};

  // Setup
  nvvk::MemAllocator m_memAllocator;
  nvvk::Allocator*   m_pAlloc;  // Allocator for buffer, images, acceleration structures
  nvvk::DebugUtil    m_debug;   // Utility to name objects
  vk::Device         m_device;
  uint32_t           m_queueFamilyIndex;

  // Resources
  std::array<nvvk::Buffer, EBuffer::eLast_elem>            m_buffers;
  std::vector<nvvk::Texture>                               m_textures;         // vector of all textures of the scene
  std::vector<std::pair<nvvk::Image, vk::ImageCreateInfo>> m_images;           // vector of all images of the scene
  std::vector<size_t>                                      m_defaultTextures;  // for cleanup

  vk::DescriptorPool      m_descPool;
  vk::DescriptorSetLayout m_descSetLayout;
  vk::DescriptorSet       m_descSet;


  void        createTextureImages(vk::CommandBuffer cmdBuf, tinygltf::Model& gltfModel);
  void        createDescriptorSet();
  std::string m_sceneName;
};
