import platform
import urllib.request
import os
import zipfile

def IsWindows():
	return platform.system() == 'Windows'

def IsLinux():
	return platform.system() == 'Linux'

def GetPlatform():
	return platform.system()

def GetArchitecture():
	return platform.architecture()[0]

def GetExecutableExtension():
	if IsWindows():
		return '.exe'
	return ''

def DoesFileExist(file_path):
	return os.path.exists(file_path)

def CreateDirectoryIfNotExists(directory_path):
	if not os.path.exists(directory_path):
		os.makedirs(directory_path)

def DownloadFile(url, output_path):
	if not os.path.exists(output_path):
		urllib.request.urlretrieve(url, output_path)
	else:
		print(f'{output_path} already exists, skipping download.')

def UnzipFile(file_path, output_folder):
	with zipfile.ZipFile(file_path, 'r') as zip_ref:
		zip_ref.extractall(output_folder)