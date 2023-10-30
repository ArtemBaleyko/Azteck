import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

VULKAN_SDK = os.environ.get('VULKAN_SDK')
AZTECK_VULKAN_VERSION = '1.3.261.1'
VULKAN_SDK_INSTALLER_URL = f'https://sdk.lunarg.com/sdk/download/{AZTECK_VULKAN_VERSION}/windows/vulkan_sdk.exe'
VULKAN_SDK_EXE_PATH = 'Azteck/vendor/VulkanSDK/VulkanSDK.exe'

def InstallVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")

def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        InstallVulkanSDK()
        quit()

def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    elif (AZTECK_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(f"You don't have the correct Vulkan SDK version! (Azteck requires {AZTECK_VULKAN_VERSION})")
        InstallVulkanPrompt()
        return False

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True
