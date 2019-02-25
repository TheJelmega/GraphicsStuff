
#include <SFML/Window/Window.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "VulkanSwapChain.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanHelpers.h"
#include "VulkanRenderTarget.h"
#include "VulkanSemaphore.h"
#include "VulkanQueue.h"

namespace Vulkan {
	
	VulkanSwapChain::VulkanSwapChain()
		: SwapChain()
		, m_Surface(VK_NULL_HANDLE)
		, m_Swapchain(VK_NULL_HANDLE)
	{
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
	}

	b8 VulkanSwapChain::Init(RHI::RHIContext* pContext, sf::Window* pWindow, RHI::VSyncMode vsync, RHI::Queue* pQueue)
	{
		VulkanInstance* pInstance = ((VulkanContext*)pContext)->GetInstance();
		VulkanDevice* pDevice = ((VulkanContext*)pContext)->GetDevice();

		VkResult vkres;

		// Only create surface if none exists yet
		if (m_Surface == VK_NULL_HANDLE)
		{
			// Set variables (in if statement, since it only needs to be done once)
			m_pContext = pContext;
			m_pWindow = pWindow;
			
			// Create surface
#if defined(VK_USE_PLATFORM_WIN32_KHR)

			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			sf::WindowHandle handle = m_pWindow->getSystemHandle();
			createInfo.hwnd = (HWND)handle;
			createInfo.hinstance = (HINSTANCE)GetWindowLongPtr((HWND)handle, GWLP_HINSTANCE);
#else
#endif
			vkres = pInstance->vkCreateSurface(createInfo, m_Surface);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create surface (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return false;
			}

			// If device exists, only update the logical device's physical device
			if (pDevice)
			{
				// Update physical device surface support
				VulkanPhysicalDevice* pPhysicalDevice = pDevice->GetPhysicalDevice();
				vkres = pPhysicalDevice->UpdateSurfaceSupport(m_Surface);
				if (vkres != VK_SUCCESS)
				{
					//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create surface (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
					Destroy();
					return false;
				}
			}
			else // No logical device exists, so update all physical devices
			{
				vkres = ((VulkanContext*)m_pContext)->UpdateSurfaceSupport(m_Surface);
				if (!vkres)
					return false;
				return true;
			}
		}

		// Create swapchain (only possible when device exists)
		if (pDevice)
		{
			m_VSync = vsync;
			m_pPresentQueue = pQueue;

			VulkanPhysicalDevice* pPhysicalDevice = pDevice->GetPhysicalDevice();
			const VulkanPhysicalDevice::SurfaceSupport& surfaceSupport = pPhysicalDevice->GetSurfaceSupport();
			VkSurfaceFormatKHR format = Helpers::GetOptimalSurfaceFormat(surfaceSupport.formats);
			VkPresentModeKHR presentMode = Helpers::GetOptimalPresentMode(surfaceSupport.modes, m_VSync);

			VkSurfaceCapabilitiesKHR capabilities;
			pPhysicalDevice->QuerySurfaceCapabilities(m_Surface, capabilities);
			sf::Vector2u windowSize = m_pWindow->getSize();
			VkExtent2D extent = Helpers::GetSwapExtent(glm::uvec2(windowSize.x, windowSize.y), capabilities);

			u32 minImages = vsync == RHI::VSyncMode::Tripple ? 3 : 2;
			minImages = std::min(std::max(minImages, capabilities.minImageCount), capabilities.maxImageCount);

			// TODO: check how to handle special params
			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = m_Surface;
			createInfo.imageFormat = format.format;
			createInfo.imageColorSpace = format.colorSpace;
			createInfo.presentMode = presentMode;
			createInfo.imageExtent = extent;
			createInfo.minImageCount = minImages;
			createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			// We don't want bending with other images for now
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.clipped = VK_TRUE;
			// For now, use basic values
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			// We made sure that the graphics queue supports present
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			// Don't set the oldSwapchain on creation
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			vkres = pDevice->vkCreateSwapchain(createInfo, m_Swapchain);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create swapchain (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				Destroy();
				return false;
			}

			// Create render targets
			u32 imageCount;
			vkres = pDevice->vkGetSwapchainImages(m_Swapchain, imageCount, nullptr);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				Destroy();
				return false;
			}
			std::vector<VkImage> images(imageCount);
			vkres = pDevice->vkGetSwapchainImages(m_Swapchain, imageCount, images.data());
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				Destroy();
				return false;
			}

			for (VkImage image : images)
			{
				VulkanRenderTarget* pRenderTarget = new VulkanRenderTarget();
				b8 res = pRenderTarget->Create(m_pContext, image, VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR, extent.width, extent.height, Helpers::GetPixelFormat(format.format), RHI::SampleCount::Sample1, RHI::RenderTargetType::Presentable);
				if (!res)
				{
					delete pRenderTarget;
					Destroy();
					return false;
				}
				m_RenderTargets.push_back(pRenderTarget);
			}

			// Create semaphores
			for (u32 i = 0; i < m_RenderTargets.size(); ++i)
			{
				// Wait semaphore
				VulkanSemaphore* pWaitSemaphore = new VulkanSemaphore();
				b8 res = pWaitSemaphore->Create(m_pContext);
				if (!res)
				{
					//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create wait semaphore for backbuffer %u!", i);
					Destroy();
					return false;
				}

				m_WaitSemaphores.push_back(pWaitSemaphore);

				// Signal semaphore
				VulkanSemaphore* pSignalSemaphore = new VulkanSemaphore();
				res = pSignalSemaphore->Create(m_pContext);
				if (!res)
				{
					//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create signal semaphore for backbuffer %u!", i);
					Destroy();
					return false;
				}

				m_SignalSemaphores.push_back(pSignalSemaphore);
			}

			// Retrieve the first image
			m_SemaphoreIndex = 0;
			VkSemaphore acquireSemaphore = ((VulkanSemaphore*)m_SignalSemaphores[m_SemaphoreIndex])->GetSemaphore();
			vkres = pDevice->vkAquireNextImage(m_Swapchain, m_RenderTargetIndex, acquireSemaphore);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to acquire initial swapchain image (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				Destroy();
				return false;
			}
		}

		return true;
	}

	b8 VulkanSwapChain::Destroy()
	{
		b8 res = true;

		// Make sure renderview resources aren't in use
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		if (pDevice)
			res = pDevice->WaitIdle();

		for (RHI::Semaphore* pSemaphore : m_SignalSemaphores)
		{
			pSemaphore->Destroy();
			delete pSemaphore;
		}
		m_SignalSemaphores.clear();

		for (RHI::Semaphore* pSemaphore : m_WaitSemaphores)
		{
			pSemaphore->Destroy();
			delete pSemaphore;
		}
		m_WaitSemaphores.clear();

		for (RHI::RenderTarget* pRenderTarget : m_RenderTargets)
		{
			b8 tmpRes = pRenderTarget->Destroy();
			if (!tmpRes)
			{
				//g_Logger.LogError("Failed to destroy render view render target!");
			}
			delete pRenderTarget;
			res &= tmpRes;
		}
		m_RenderTargets.clear();

		if (m_Swapchain)
		{
			pDevice->vkDestroySwapchain(m_Swapchain);
			m_Swapchain = VK_NULL_HANDLE;
		}

		if (m_Surface)
		{
			VulkanInstance* pInstance = ((VulkanContext*)m_pContext)->GetInstance();
			pInstance->vkDestroySurface(m_Surface);
			m_Surface = VK_NULL_HANDLE;
		}
		return res;
	}

	b8 VulkanSwapChain::Present()
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_RenderTargetIndex;

		VkSemaphore semaphore = ((VulkanSemaphore*)m_WaitSemaphores[m_SemaphoreIndex])->GetSemaphore();
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &semaphore;

		VkResult vkres = ((VulkanQueue*)m_pPresentQueue)->vkPresent(presentInfo);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to present a vulkan swapchain (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			Destroy();
			return false;
		}

		++m_SemaphoreIndex %= m_RenderTargets.size();
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkSemaphore acquireSemaphore = ((VulkanSemaphore*)m_SignalSemaphores[m_SemaphoreIndex])->GetSemaphore();
		vkres = pDevice->vkAquireNextImage(m_Swapchain, m_RenderTargetIndex, acquireSemaphore);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to acquire initial swapchain image (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			Destroy();
			return false;
		}
		
		return true;
	}

	void VulkanSwapChain::ResizeCallback(u32 width, u32 height)
	{
		(void)width;
		(void)height;
		Destroy();
		Init(m_pContext, m_pWindow, m_VSync, m_pPresentQueue);
	}

}
