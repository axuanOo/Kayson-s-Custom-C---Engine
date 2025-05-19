#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <string>
#include <windows.h>
#include <unordered_set>
#include <functional>

struct VertexIndexStructure
{
	int		m_vertPositionIndex = -1;
	int		m_vertTextureCoordsIndex = -1;
	int		m_vertNormalIndex = -1;

	unsigned int m_vertexPositionInArray = 0;

	bool operator ==(VertexIndexStructure const& other) const
	{
		return m_vertPositionIndex == other.m_vertPositionIndex &&
			m_vertTextureCoordsIndex == other.m_vertTextureCoordsIndex &&
			m_vertNormalIndex == other.m_vertNormalIndex;
	}
};
struct VertexIndexHasher
{
	std::size_t operator()(const VertexIndexStructure& vertex) const
	{
		std::size_t hash1 = std::hash<int>()(vertex.m_vertPositionIndex);
		std::size_t hash2 = std::hash<int>()(vertex.m_vertTextureCoordsIndex);
		std::size_t hash3 = std::hash<int>()(vertex.m_vertNormalIndex);

		return hash1 ^ (hash2 << 1) ^ (hash3 << 2);
	}
};

bool ObjLoader::Load(const std::string& fileName, std::vector<Vertex_PCUTBN>& out_Vertexes, std::vector<unsigned int>& out_Indexes, bool& out_hasNormal, bool& out_hasUVs, const Mat44& transform /*= Mat44()*/)
{
	std::vector<uint8_t> buffer;

	auto total_start_time = std::chrono::high_resolution_clock::now();

	FileReadToBinary(buffer, fileName);
	std::string bufferString = std::string(buffer.begin(), buffer.end());

	Strings strings = SplitStringOnDelimiter(bufferString, "\n", true);
	out_Vertexes.reserve(strings.size());
	out_Indexes.reserve(strings.size());

	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;
	std::map<std::string, Rgba8> materials;
	std::unordered_set<VertexIndexStructure, VertexIndexHasher> vertsDictionary;

	Rgba8 currentMatColor = Rgba8::WHITE;

	int vertex_count = 0;
	int uv_count = 0;
	int normal_count = 0;
	int face_count = 0;
	int triangle_count = 0;

	for (int stringIndex = 0; stringIndex < (int)strings.size(); stringIndex++) {
		if (!strings[stringIndex].empty() && strings[stringIndex].back() == '\r') {
			strings[stringIndex].erase(strings[stringIndex].size() - 1);
		}

		Strings stringPiece = SplitStringOnDelimiter(strings[stringIndex], " ", true);

		if (!stringPiece.empty()) {
			if (stringPiece[0] == "v") {
				Vec3 position = Vec3(std::stof(stringPiece[1].c_str()), std::stof(stringPiece[2].c_str()), std::stof(stringPiece[3].c_str()));
				positions.push_back(position);
				vertex_count++;
			}

			if (stringPiece[0] == "vt") {
				out_hasUVs = true;
				Vec2 uvVector = Vec2(std::stof(stringPiece[1].c_str()), std::stof(stringPiece[2].c_str()));
				uvs.push_back(uvVector);
				uv_count++;
			}

			if (stringPiece[0] == "vn") {
				out_hasNormal = true;
				Vec3 normalVector = Vec3(std::stof(stringPiece[1].c_str()), std::stof(stringPiece[2].c_str()), std::stof(stringPiece[3].c_str()));
				normals.push_back(normalVector);
				normal_count++;
			}

			if (stringPiece[0] == "f") 
			{
				std::vector<VertexIndexStructure> tmpVerts;

				for (int i = 1; i < (int)stringPiece.size(); i++) 
				{
					VertexIndexStructure vert = VertexIndexStructure();

					Strings subFaceString = SplitStringOnDelimiter(stringPiece[i], "/", false);

					if (subFaceString.size() > 0 && !subFaceString[0].empty()) {
						uint32_t posIndex = std::stoi(subFaceString[0].c_str()) - 1;
						vert.m_vertPositionIndex = posIndex;
					}

					if (subFaceString.size() > 1 && !subFaceString[1].empty()) {
						uint32_t uvIndex = std::stoi(subFaceString[1].c_str()) - 1;
						vert.m_vertTextureCoordsIndex = uvIndex;
					}

					if (subFaceString.size() > 2 && !subFaceString[2].empty()) {
						uint32_t normalIndex = std::stoi(subFaceString[2].c_str()) - 1;
						vert.m_vertNormalIndex = normalIndex;
					}
					if (vert.m_vertPositionIndex != -1)
					{

						auto it = vertsDictionary.find(vert);
						if (it == vertsDictionary.end())
						{
							vert.m_vertexPositionInArray = (unsigned int)out_Vertexes.size();

							Vertex_PCUTBN tbnVert;
							tbnVert.m_position = positions[vert.m_vertPositionIndex];
							if (vert.m_vertTextureCoordsIndex != -1)
							{
								tbnVert.m_uvTexCoords = uvs[vert.m_vertTextureCoordsIndex];
							}
							tbnVert.m_color = currentMatColor;
							if (vert.m_vertNormalIndex != -1)
							{
								tbnVert.m_normal = normals[vert.m_vertNormalIndex];
							}
							vertsDictionary.insert(vert);
							out_Vertexes.push_back(tbnVert);
						}
						else 
						{
							vert.m_vertexPositionInArray = it->m_vertexPositionInArray;
						}
						tmpVerts.push_back(vert);
					}
				}

				for (int i = 1; i < (int)tmpVerts.size() - 1; i++) 
				{
					out_Indexes.push_back(tmpVerts[0].m_vertexPositionInArray);

					out_Indexes.push_back(tmpVerts[i].m_vertexPositionInArray);

					out_Indexes.push_back(tmpVerts[i + 1].m_vertexPositionInArray);

					triangle_count++;
				}
				face_count++;
			}

			if (stringPiece[0] == "mtllib") {
				std::filesystem::path filePath(fileName);
				std::filesystem::path directory = filePath.parent_path();
				std::filesystem::path mtlFilePath = directory / stringPiece[1];
				std::string strPath = mtlFilePath.string();

				std::vector<uint8_t> mtlBuffer;
				FileReadToBinary(mtlBuffer, strPath);
				std::string mtlBufferString = std::string(mtlBuffer.begin(), mtlBuffer.end());

				Strings mtlBufferStrings = SplitStringOnDelimiter(mtlBufferString, "\n", true);
				std::string currentMtlName;

				for (int mtlIndex = 0; mtlIndex < (int)mtlBufferStrings.size(); mtlIndex++) {
					if (!mtlBufferStrings[mtlIndex].empty() && mtlBufferStrings[mtlIndex].back() == '\r') {
						mtlBufferStrings[mtlIndex].erase(mtlBufferStrings[mtlIndex].size() - 1);
					}

					Strings mtlStrings = SplitStringOnDelimiter(mtlBufferStrings[mtlIndex], " ");

					if (mtlStrings[0] == "newmtl") {
						currentMtlName = mtlStrings[1];
					}

					if (mtlStrings[0] == "Kd") {
						if (!currentMtlName.empty()) {
							float rNorm = std::stof(mtlStrings[1]);
							float gNorm = std::stof(mtlStrings[2]);
							float bNorm = std::stof(mtlStrings[3]);
							materials[currentMtlName] = Rgba8(DenormalizeByte(rNorm), DenormalizeByte(gNorm), DenormalizeByte(bNorm));
						}
					}
				}
			}

			if (stringPiece[0] == "usemtl") {
				if (materials.find(stringPiece[1]) != materials.end()) {
					currentMatColor = materials[stringPiece[1]];
				}
			}
		}
	}

	TransformVertexArray3D(out_Vertexes, transform);
	return true;
}
