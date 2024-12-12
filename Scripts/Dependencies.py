import Utils
import os
import subprocess
import sys
import webbrowser

def Install():
	print("Installing Dependencies")
	if IsVulkanSDKInstalled() == False:
		InstallVulkanSDK()

VULKAN_SDK_PATH = os.environ.get('VULKAN_SDK')
VULKAN_REQUIRED_VERSION = '1.3'
VULKAN_INSTALL_VERSION = '1.3.296.0'
VULKAN_DOWNLOAD_URL = f'https://vulkan.lunarg.com/sdk/home'

def IsVulkanSDKInstalled():
	if VULKAN_SDK_PATH is None:
		return False
	elif not VULKAN_REQUIRED_VERSION in VULKAN_SDK_PATH:
		return False
	elif not Utils.DoesDirectoryExist(VULKAN_SDK_PATH):
		return False
	return True

def InstallVulkanSDK():
	print('Vulkan SDK not found, installing...')
	webbrowser.open(VULKAN_DOWNLOAD_URL, new=0, autoraise=True)
	print('Opening browser to ' + VULKAN_DOWNLOAD_URL + '...')
	print('')
	print('Please download and install the Vulkan SDK from ' + VULKAN_DOWNLOAD_URL)
	print('Download vulkan sdk version ' + VULKAN_INSTALL_VERSION + ' rerun the setup script after the Vulkan SDK installation has completed.')
	sys.exit(1)
