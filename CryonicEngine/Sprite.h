#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "RaylibWrapper.h"
#if defined (EDITOR)
#include "ProjectManager.h"
#else
#include "Game.h"
#endif

class Sprite
{
public:
	Sprite(std::string path)
	{
//		std::ifstream file;
//#ifndef EDITOR
//		file.open(std::filesystem::path(exeParent) / "Resources" / "Assets" / std::string(path + ".data"));
//#endif
//		if (!file.is_open())
//		{
//			ConsoleLogger::ErrorLog("Sprite failed to load. Path: " + path);
//			return;
//		}

		relativePath = path;

		if (path != "Square" && path != "Circle" && path != "None")
		{
#if defined(EDITOR)
			path = ProjectManager::projectData.path.string() + "/Assets/" + path;
#else
			path = exeParent.string() + "/Resources/Assets/" + path;
#endif

			if (auto it = textures.find(path); it != textures.end())
				texture = it->second.first;
			else
			{
				textures[path].first = new RaylibWrapper::Texture2D(RaylibWrapper::LoadTexture(path.c_str()));
				texture = textures[path].first;
			}
		}

		this->path = path;
	}

	/**
	 * @brief Returns the full path to the sprite file.
	 *
	 * @return [string] The full path to the sprite file.
	 */
	std::string GetPath() const { return path; };

	/**
	* @brief Returns the relative path to the sprite file.
	*
	* @return [string] The relative path to the sprite file.
	*/
	std::string GetRelativePath() const { return relativePath; };

	// Hide in API
	RaylibWrapper::Texture2D* GetTexture() const { return texture; };

private:
	std::string path;
	std::string relativePath;
	RaylibWrapper::Texture2D* texture = nullptr;
	static std::unordered_map<std::filesystem::path, std::pair<RaylibWrapper::Texture2D*, int>> textures;
};