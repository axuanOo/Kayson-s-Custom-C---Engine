#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/OBB2.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
{
	std::string pathNoExtension = std::string(fontFilePathNameWithNoExtension);
	pathNoExtension.append(".png");

	Image img = Image(pathNoExtension.c_str());
	IntVec2 texDim = img.GetDimensions();
	
	int columns = 16;
	int rows = 16;
	int cellWidth = texDim.x / columns;
	int cellHeight = texDim.y / rows;

	int startChar = static_cast<int>('!');  // 33
	int endChar = static_cast<int>('~');  

	auto GetAlpha = [&](int x, int y) {
		return img.GetTexelColor(IntVec2(x, y)).a;
		};
	
	for (int ch = startChar; ch <= endChar; ++ch)
	{
		int colIdx = ch % columns;
		int rowIdx = ch / columns;

		int cellX = colIdx * cellWidth;
		int cellY = (rows - 1 - rowIdx) * cellHeight;

		int minX = cellWidth;
		int minY = cellHeight;
		int maxX = -1;
		int maxY = -1;

		for (int x = 0; x < cellWidth; ++x) {
			for (int y = 0; y < cellHeight; ++y) {
				if (GetAlpha(cellX + x, cellY + y) > 0) {
					minX = std::min(minX, x);
					minY = std::min(minY, y);
					maxX = std::max(maxX, x);
					maxY = std::max(maxY, y);
				}
			}
		}
		if (maxX < minX) {
			continue;
		}

		m_glyphUOffset[ch] = minX / float(cellWidth);
		m_glyphUWidth[ch] = (maxX - minX + 1) / float(cellWidth);
	}
	m_fontGlyphsSpriteSheet = new SpriteSheet(fontTexture,IntVec2(16,16));
	m_fontFilePathNameWithNoExtension = std::string(fontFilePathNameWithNoExtension);
}

BitmapFont::BitmapFont(char const* xmlPath, char const* fontPath, Texture& fontTexture)
{
	m_fontFilePathNameWithNoExtension = std::string(fontPath);
	
	Image img = Image(fontPath);
	m_textureDimensions = img.GetDimensions();

	XmlDocument xmlDoc;
	xmlDoc.LoadFile(xmlPath);

	XmlElement* root = xmlDoc.RootElement();
	XmlElement* charsElement = root->FirstChildElement("chars");

	XmlElement* charElement = charsElement->FirstChildElement("char");
	while (charElement)
	{
		FontMetaData info;
		info.m_id = ParseXmlAttribute(*charElement, "id", 0);
		info.m_x = ParseXmlAttribute(*charElement, "x", 0);
		info.m_y = ParseXmlAttribute(*charElement, "y", 0);
		info.m_width = ParseXmlAttribute(*charElement, "width", 0);
		info.m_height = ParseXmlAttribute(*charElement, "height", 0);
		info.m_xOffset = ParseXmlAttribute(*charElement, "xoffset", 0);
		info.m_yOffset = ParseXmlAttribute(*charElement, "yoffset", 0);

		m_customFontMap[(unsigned char)(info.m_id)] = info;

		charElement = charElement->NextSiblingElement("char");
	}
	
	m_fontGlyphsSpriteSheet = new SpriteSheet(fontTexture, IntVec2(16, 16));
}


Texture const& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet->GetTexture();
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, const float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/)
{
	int vertsNum = (int)text.size() * 6;
	vertexArray.reserve(vertsNum);

	float cellWidth = cellHeight * cellAspect;

	Vec2 currentTextPosition = textMins;
	
	for (int charIndex = 0; charIndex < text.size(); ++charIndex)
	{
		int asciiValue = static_cast<int>(text[charIndex]);
		const SpriteDefinition& spriteOfLetter = m_fontGlyphsSpriteSheet->GetSpriteDef(asciiValue);

		Vec2 leftBottomPoint = currentTextPosition + Vec2(-0.5f * cellWidth, -0.5f * cellHeight);
		Vec2 rightBottomPoint = currentTextPosition + Vec2(0.5f * cellWidth, -0.5f * cellHeight);
		Vec2 leftTopPoint = currentTextPosition + Vec2(-0.5f * cellWidth, 0.5f * cellHeight);
		Vec2 rightTopPoint = currentTextPosition + Vec2(0.5f * cellWidth, 0.5f * cellHeight);

		AABB2 uvBoundsOfSpriteDef = spriteOfLetter.GetUVs();

		Vec2 leftBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 0.0f));
		Vec2 rightBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 0.0f));
		Vec2 leftTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 1.0f));
		Vec2 rightTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 1.0f));

		Vertex_PCU leftBottomPCU = Vertex_PCU(Vec3(leftBottomPoint.x,leftBottomPoint.y,0.0f),tint,leftBottomUVs);
		Vertex_PCU rightBottomPCU = Vertex_PCU(Vec3(rightBottomPoint.x, rightBottomPoint.y, 0.0f), tint, rightBottomUVs);
		Vertex_PCU leftTopPCU = Vertex_PCU(Vec3(leftTopPoint.x, leftTopPoint.y, 0.0f), tint, leftTopUVs);
		Vertex_PCU rightTopPCU = Vertex_PCU(Vec3(rightTopPoint.x, rightTopPoint.y, 0.0f), tint, rightTopUVs);

		vertexArray.push_back(leftBottomPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(leftTopPCU);

		vertexArray.push_back(leftTopPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(rightTopPCU);

		currentTextPosition += Vec2(cellWidth, 0.0f);
	}
}

void BitmapFont::AddVertsForText2DTier2(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, const float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/)
{
	int vertsNum = (int)text.size() * 6;
	vertexArray.reserve(vertsNum);

	Vec2 currentTextPosition = textMins;

	for (int charIndex = 0; charIndex < text.size(); ++charIndex)
	{
		int asciiValue = static_cast<int>(text[charIndex]);
		float height = cellHeight * m_glyphUWidth[asciiValue];
		if (m_glyphUWidth[asciiValue] == 0.0f)
		{
			height = cellHeight;
		}
		float cellWidth = height * cellAspect;
		const SpriteDefinition& spriteOfLetter = m_fontGlyphsSpriteSheet->GetSpriteDef(asciiValue);

		Vec2 leftBottomPoint = currentTextPosition + Vec2(-0.5f * cellWidth, -0.5f * cellHeight);
		Vec2 rightBottomPoint = currentTextPosition + Vec2(0.5f * cellWidth, -0.5f * cellHeight);
		Vec2 leftTopPoint = currentTextPosition + Vec2(-0.5f * cellWidth, 0.5f * cellHeight);
		Vec2 rightTopPoint = currentTextPosition + Vec2(0.5f * cellWidth, 0.5f * cellHeight);

		AABB2 uvBoundsOfSpriteDef = spriteOfLetter.GetUVs();

		Vec2 leftBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 0.0f));
		Vec2 rightBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 0.0f));
		Vec2 leftTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 1.0f));
		Vec2 rightTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 1.0f));

		Vertex_PCU leftBottomPCU = Vertex_PCU(Vec3(leftBottomPoint.x, leftBottomPoint.y, 0.0f), tint, leftBottomUVs);
		Vertex_PCU rightBottomPCU = Vertex_PCU(Vec3(rightBottomPoint.x, rightBottomPoint.y, 0.0f), tint, rightBottomUVs);
		Vertex_PCU leftTopPCU = Vertex_PCU(Vec3(leftTopPoint.x, leftTopPoint.y, 0.0f), tint, leftTopUVs);
		Vertex_PCU rightTopPCU = Vertex_PCU(Vec3(rightTopPoint.x, rightTopPoint.y, 0.0f), tint, rightTopUVs);

		vertexArray.push_back(leftBottomPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(leftTopPCU);

		vertexArray.push_back(leftTopPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(rightTopPCU);

		currentTextPosition += Vec2(cellWidth, 0.0f);
	}
}

void BitmapFont::AddVertsForText2DTier3(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8(255, 255, 255, 255)*/, float spaceBetweenTwoChar /*= 1.f*/)
{
	AABB2 lastTextBound(textMins, textMins);
	float standardHeight = cellHeight;
	float baseWidth = m_customFontMap[(unsigned char)'A'].m_width * (standardHeight / 72.f);
	float ratio = standardHeight / 72.f;
	float spacing = spaceBetweenTwoChar * standardHeight;

	for (int i = 0; i < (int)text.size(); i++)
	{
		unsigned char c = static_cast<unsigned char>(text[i]);
		FontMetaData& fontInfo = m_customFontMap[c];

		float cw = (c == ' ') ? baseWidth : fontInfo.m_width * ratio;
		float chh = fontInfo.m_height * ratio;

		float offsetY = fontInfo.m_yOffset * ratio;

		Vec2 minPos = Vec2(lastTextBound.m_maxs.x + spacing, textMins.y);
		Vec2 maxPos = Vec2(lastTextBound.m_maxs.x + cw + spacing, textMins.y + chh);
		AABB2 textBound(minPos, maxPos);
		textBound.Translate(Vec2(0.0f, -offsetY));

		Vec2 uvMin(
			fontInfo.m_x / (float)m_textureDimensions.x,
			1.f - (fontInfo.m_y + fontInfo.m_height) / (float)m_textureDimensions.y
		);
		Vec2 uvMax(
			(fontInfo.m_x + fontInfo.m_width) / (float)m_textureDimensions.x,
			1.f - fontInfo.m_y / (float)m_textureDimensions.y
		);
		AABB2 uvBound(uvMin, uvMax);

		if (c != ' ')
		{
			AddVertsForAABB2D(vertexArray, textBound, tint, uvBound.m_mins, uvBound.m_maxs);
		}

		lastTextBound.m_mins.x = textBound.m_mins.x;
		lastTextBound.m_maxs.x = textBound.m_maxs.x;
	}
}

void BitmapFont::AddVertsForText2DTier3AlongAxis(std::vector<Vertex_PCU>& vertexArray, Vec2 const& direction, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8(255, 255, 255, 255)*/, float spaceBetweenTwoChar /*= 0.2f*/)
{
	Vec2 dirNorm = direction.GetNormalized();
	Vec2 perp = Vec2(-dirNorm.y, dirNorm.x);

	float standardHeight = cellHeight;
	float baseWidth = m_customFontMap[(unsigned char)'A'].m_width * (standardHeight / 72.f);
	float ratio = standardHeight / 72.f;
	float spacing = spaceBetweenTwoChar * standardHeight;

	Vec2 pen = textMins;

	for (char ch : text)
	{
		unsigned char c = static_cast<unsigned char>(ch);
		FontMetaData& fontInfo = m_customFontMap[c];

		float cw = (c == ' ') ? baseWidth : fontInfo.m_width * ratio;
		float chh = fontInfo.m_height * ratio;
		float offsetY = fontInfo.m_yOffset * ratio;

		pen += dirNorm * spacing;

		Vec2 charOrigin = pen - perp * offsetY;

		OBB2 obb;
		obb.m_halfDimensions = Vec2(cw * 0.5f, chh * 0.5f);
		obb.m_iBasisNormal = dirNorm;
		obb.m_center = charOrigin + dirNorm * (cw * 0.5f) + perp * (chh * 0.5f);

		Vec2 uvMin(
			fontInfo.m_x / (float)m_textureDimensions.x,
			1.f - (fontInfo.m_y + fontInfo.m_height) / (float)m_textureDimensions.y
		);
		Vec2 uvMax(
			(fontInfo.m_x + fontInfo.m_width) / (float)m_textureDimensions.x,
			1.f - fontInfo.m_y / (float)m_textureDimensions.y
		);
		AABB2 uvBounds(uvMin, uvMax);

		if (c != ' ')
		{
			AddVertsForOBB2D(vertexArray, obb, tint, uvBounds);
		}

		pen += dirNorm * cw;
	}

}

void BitmapFont::AddVertsForText2DTier5(std::vector<Vertex_Font>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, float spaceBetweenTwoChar/* = 0.2f*/, Rgba8 const& tint /*= Rgba8(255, 255, 255, 255)*/, float cellAspect /*= 1.0f*/, const Vec2& glyphPosition /*= Vec2(0.f, 0.f)*/, const Vec2& textPosition /*= Vec2(0.f, 0.f)*/, int characterIndex /*= 0*/, float weight /*= 0.f*/)
{	
	UNUSED(cellAspect);
	Vec2 pen = textMins;
	int  N = (int)text.size();
	static const int tris[6] = { 0,1,2,  2,1,3 };
	float scale = cellHeight / 72.f;

	for (int i = 0; i < N; ++i)
	{
		unsigned char c = (unsigned char)text[i];
		auto& fm = m_customFontMap[c];

		float glyphW = fm.m_width * scale;
		float glyphH = fm.m_height * scale;
		float halfW = glyphW * 0.5f;
		float halfH = glyphH * 0.5f;

		Vec2 uvMin(
			fm.m_x / (float)m_textureDimensions.x,
			1.f - (fm.m_y + fm.m_height) / (float)m_textureDimensions.y
		);
		Vec2 uvMax(
			(fm.m_x + fm.m_width) / (float)m_textureDimensions.x,
			1.f - fm.m_y / (float)m_textureDimensions.y
		);
		AABB2 uvRect(uvMin, uvMax);

		Vec2 offsets[4] = {
			Vec2(-halfW, -halfH),
			Vec2(halfW, -halfH),
			Vec2(-halfW,  halfH),
			Vec2(halfW,  halfH),
		};

		Vec2 glyphNorm[4] = {
			Vec2(0.f,0.f), Vec2(1.f,0.f),
			Vec2(0.f,1.f), Vec2(1.f,1.f),
		};

		float tx = (N > 1) ? float(i) / (N - 1) : 0.f;
		Vec2 textNorm[4] = {
			Vec2(tx,0.f), Vec2(tx,0.f),
			Vec2(tx,1.f), Vec2(tx,1.f),
		};

		for (int v = 0; v < 6; ++v)
		{
			int   idx = tris[v];
			Vec2  pos2D = pen + offsets[idx];
			Vec3  pos3D = Vec3(pos2D.x, pos2D.y, 0.f);
			Vec2  uv = uvRect.GetPointAtUV(glyphNorm[idx]);

			Vec2  finalGP = glyphPosition + glyphNorm[idx];
			Vec2  finalTP = textPosition + textNorm[idx];
			int   finalCI = characterIndex;

			vertexArray.emplace_back(
				pos3D,
				tint,
				uv,
				finalGP,  
				finalTP,  
				finalCI,  
				weight
			);
		}

		pen.x += glyphW + spaceBetweenTwoChar * cellHeight;
	}
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, Vec2 const& alignment /*= Vec2(.5f, .5f)*/, TextBoxMode mode /*= TextBoxMode::SHRINK_TO_FIT*/, int maxGlyphsToDraw /*= 99999999*/, float spacingRatio /*= 1.0f*/, float verticalLineSpacing, bool autoWrap)
{
	if (autoWrap)
	{
		int vertsNum = (int)text.size() * 6;
		vertexArray.reserve(vertsNum);

		float cellWidth = cellHeight * cellAspect;

		Strings stringsInLines = SplitStringOnDelimiter(text, '\n');

		std::vector<std::string> mergedLines;
		for (const auto& line : stringsInLines) {
			mergedLines.push_back(line);
		}

		std::vector<std::string> finalLines;

		for (const auto& line : mergedLines) {
			std::string currentLine = line;
			while (!currentLine.empty()) {
				if (box.GetDimensions().x < (currentLine.size() * cellWidth)) {
					int breakIndex = static_cast<int>(box.GetDimensions().x / cellWidth);
					finalLines.push_back(currentLine.substr(0, breakIndex));
					currentLine = currentLine.substr(breakIndex);
				}
				else {
					finalLines.push_back(currentLine);
					break;
				}
			}
		}

		int maxElement = 0;
		for (const auto& line : finalLines) {
			if (line.size() > maxElement) {
				maxElement = static_cast<int>(line.size());
			}
		}

		for (int stringIndex = 0; stringIndex < finalLines.size(); ++stringIndex) {
			Vec2 startLinePosition = box.GetPointAtUV(Vec2(0.0f, 1.0f));
			startLinePosition += static_cast<float>(stringIndex) * Vec2(0.0f, -cellHeight * verticalLineSpacing);
			startLinePosition += 0.5f * Vec2(cellWidth, -cellHeight);

			const std::string& stringThisLine = finalLines[stringIndex];

			float lineWidth = cellWidth * stringThisLine.size();
			float lineHeight = cellHeight * finalLines.size();

			float lineAndBoxHorizontalOffset = box.GetDimensions().x - lineWidth;
			float lineAndBoxVerticalOffset = box.GetDimensions().y - lineHeight;

			startLinePosition += alignment.x * Vec2(lineAndBoxHorizontalOffset, 0.0f);
			startLinePosition += alignment.y * Vec2(0.0f, -lineAndBoxVerticalOffset);

			int maxLettersToDraw = maxGlyphsToDraw;

			for (int charIndex = 0; charIndex < stringThisLine.size(); ++charIndex) {
				if (maxLettersToDraw <= 0) {
					break;
				}

				Vec2 charPosition = startLinePosition + static_cast<float>(charIndex) * Vec2(cellWidth * spacingRatio, 0.0f);
				const char& letter = stringThisLine[charIndex];
				int asciiValue = static_cast<int>(letter);
				const SpriteDefinition& spriteOfLetter = m_fontGlyphsSpriteSheet->GetSpriteDef(asciiValue);

				Vec2 leftBottomPoint = charPosition + Vec2(-0.5f * cellWidth, -0.5f * cellHeight);
				Vec2 rightBottomPoint = charPosition + Vec2(0.5f * cellWidth, -0.5f * cellHeight);
				Vec2 leftTopPoint = charPosition + Vec2(-0.5f * cellWidth, 0.5f * cellHeight);
				Vec2 rightTopPoint = charPosition + Vec2(0.5f * cellWidth, 0.5f * cellHeight);

				if (mode == TextBoxMode::SHRINK_TO_FIT) {
					float maxWidth = cellWidth * maxElement;
					float maxHeight = cellHeight * finalLines.size();
					float widthShrinkStrength = box.GetDimensions().x / maxWidth;
					float heightShrinkStrength = box.GetDimensions().y / maxHeight;
					float shrinkParameter = std::min(widthShrinkStrength, heightShrinkStrength);

					if (shrinkParameter <= 1.0f) {
						Vec2 pivot = box.GetPointAtUV(Vec2(alignment.x, 1.0f - alignment.y));
						Vec2 pivotToPositionLB = Vec2(leftBottomPoint.x - pivot.x, leftBottomPoint.y - pivot.y);
						Vec2 pivotToPositionRB = Vec2(rightBottomPoint.x - pivot.x, rightBottomPoint.y - pivot.y);
						Vec2 pivotToPositionLT = Vec2(leftTopPoint.x - pivot.x, leftTopPoint.y - pivot.y);
						Vec2 pivotToPositionRT = Vec2(rightTopPoint.x - pivot.x, rightTopPoint.y - pivot.y);

						pivotToPositionLB.SetLength(pivotToPositionLB.GetLength() * shrinkParameter);
						pivotToPositionRB.SetLength(pivotToPositionRB.GetLength() * shrinkParameter);
						pivotToPositionLT.SetLength(pivotToPositionLT.GetLength() * shrinkParameter);
						pivotToPositionRT.SetLength(pivotToPositionRT.GetLength() * shrinkParameter);

						leftBottomPoint = pivot + pivotToPositionLB;
						rightBottomPoint = pivot + pivotToPositionRB;
						leftTopPoint = pivot + pivotToPositionLT;
						rightTopPoint = pivot + pivotToPositionRT;
					}
				}

				Vec3 leftBottomPoint3D = Vec3(leftBottomPoint.x, leftBottomPoint.y, 0.0f);
				Vec3 rightBottomPoint3D = Vec3(rightBottomPoint.x, rightBottomPoint.y, 0.0f);
				Vec3 leftTopPoint3D = Vec3(leftTopPoint.x, leftTopPoint.y, 0.0f);
				Vec3 rightTopPoint3D = Vec3(rightTopPoint.x, rightTopPoint.y, 0.0f);

				AABB2 uvBoundsOfSpriteDef = spriteOfLetter.GetUVs();
				Vec2 leftBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 0.0f));
				Vec2 rightBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 0.0f));
				Vec2 leftTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 1.0f));
				Vec2 rightTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 1.0f));

				Vertex_PCU leftBottomPCU = Vertex_PCU(leftBottomPoint3D, tint, leftBottomUVs);
				Vertex_PCU rightBottomPCU = Vertex_PCU(rightBottomPoint3D, tint, rightBottomUVs);
				Vertex_PCU leftTopPCU = Vertex_PCU(leftTopPoint3D, tint, leftTopUVs);
				Vertex_PCU rightTopPCU = Vertex_PCU(rightTopPoint3D, tint, rightTopUVs);

				vertexArray.push_back(leftBottomPCU);
				vertexArray.push_back(rightBottomPCU);
				vertexArray.push_back(rightTopPCU);

				vertexArray.push_back(leftBottomPCU);
				vertexArray.push_back(rightTopPCU);
				vertexArray.push_back(leftTopPCU);

				maxLettersToDraw -= 1;
			}
		}
		return;
	}

	int vertsNum = (int)text.size() * 6;
	vertexArray.reserve(vertsNum);

	float cellWidth = cellHeight * cellAspect;

	Strings stringsInLines = SplitStringOnDelimiter(text,'\n');
	
	int maxElement = (int)stringsInLines[0].size();

	for (int stringIndex = 0; stringIndex < stringsInLines.size(); ++stringIndex)
	{
		if (stringsInLines[stringIndex].size() > maxElement)
		{
			maxElement = (int)stringsInLines[stringIndex].size();
		}
	}
	
	for (int stringIndex = 0; stringIndex < stringsInLines.size(); ++stringIndex)
	{
		// Set the beginning string position at the top of the text box
		Vec2 startLinePosition = box.GetPointAtUV(Vec2(0.0f, 1.0f));
		startLinePosition += (float)stringIndex * Vec2(0.0f, -cellHeight * verticalLineSpacing);
		startLinePosition += 0.5f * Vec2(cellWidth, -cellHeight);

		const std::string& stringThisLine = stringsInLines[stringIndex];

		float lineWidth = cellWidth * stringThisLine.size();
		float lineHeight = cellHeight * stringsInLines.size(); // Fixed value

		float lineAndBoxHorizontalOffset = box.GetDimensions().x - lineWidth;
		float lineAndBoxVerticalOffset = box.GetDimensions().y - lineHeight;

		startLinePosition += alignment.x * Vec2(lineAndBoxHorizontalOffset, 0.0f);
		startLinePosition += alignment.y * Vec2(0.0f, -lineAndBoxVerticalOffset);

		///---------------------------------------------------------------------------------------
		int maxLettersToDraw = maxGlyphsToDraw;

		for (int charIndex = 0; charIndex < stringThisLine.size(); ++charIndex)
		{
			if (maxLettersToDraw<=0) 
			{
				break;
			}

			Vec2 charPosition = startLinePosition + (float)charIndex * Vec2(cellWidth * spacingRatio, 0.0f);

			const char& letter = stringThisLine[charIndex];

			int asciiValue = static_cast<int>(letter);

			const SpriteDefinition& spriteOfLetter = m_fontGlyphsSpriteSheet->GetSpriteDef(asciiValue);


			Vec2 leftBottomPoint = charPosition + Vec2(-0.5f * cellWidth, -0.5f * cellHeight);
			Vec2 rightBottomPoint = charPosition + Vec2(0.5f * cellWidth, -0.5f * cellHeight);
			Vec2 leftTopPoint = charPosition + Vec2(-0.5f * cellWidth, 0.5f * cellHeight);
			Vec2 rightTopPoint = charPosition + Vec2(0.5f * cellWidth, 0.5f * cellHeight);


			if (mode == TextBoxMode::SHRINK_TO_FIT)
			{
				float maxWidth = cellWidth * maxElement;
				float maxHeight = cellHeight * stringsInLines.size();

				float widthShrinkStrength = box.GetDimensions().x / maxWidth;
				float heightShrinkStrength = box.GetDimensions().y / maxHeight;

				float shrinkParameter = widthShrinkStrength < heightShrinkStrength ? widthShrinkStrength : heightShrinkStrength;

				if (shrinkParameter <= 1.0f)
				{
					Vec2 pivot = box.GetPointAtUV(Vec2(alignment.x, 1.0f - alignment.y));

					Vec2 pivotToPositionLB = Vec2(leftBottomPoint.x - pivot.x, leftBottomPoint.y - pivot.y);
					Vec2 pivotToPositionRB = Vec2(rightBottomPoint.x - pivot.x, rightBottomPoint.y - pivot.y);
					Vec2 pivotToPositionLT = Vec2(leftTopPoint.x - pivot.x, leftTopPoint.y - pivot.y);
					Vec2 pivotToPositionRT = Vec2(rightTopPoint.x - pivot.x, rightTopPoint.y - pivot.y);

					float pivotToPositionLengthLB = pivotToPositionLB.GetLength();
					float pivotToPositionLengthRB = pivotToPositionRB.GetLength();
					float pivotToPositionLengthLT = pivotToPositionLT.GetLength();
					float pivotToPositionLengthRT = pivotToPositionRT.GetLength();

					pivotToPositionLB.SetLength(pivotToPositionLengthLB * shrinkParameter);
					pivotToPositionRB.SetLength(pivotToPositionLengthRB * shrinkParameter);
					pivotToPositionLT.SetLength(pivotToPositionLengthLT * shrinkParameter);
					pivotToPositionRT.SetLength(pivotToPositionLengthRT * shrinkParameter);

					Vec2 tmpPosition2DLB = pivot + pivotToPositionLB;
					Vec2 tmpPosition2DRB = pivot + pivotToPositionRB;
					Vec2 tmpPosition2DLT = pivot + pivotToPositionLT;
					Vec2 tmpPosition2DRT = pivot + pivotToPositionRT;

					leftBottomPoint.x = tmpPosition2DLB.x;
					leftBottomPoint.y = tmpPosition2DLB.y;
					rightBottomPoint.x = tmpPosition2DRB.x;
					rightBottomPoint.y = tmpPosition2DRB.y;
					leftTopPoint.x = tmpPosition2DLT.x;
					leftTopPoint.y = tmpPosition2DLT.y;
					rightTopPoint.x = tmpPosition2DRT.x;
					rightTopPoint.y = tmpPosition2DRT.y;
				}
			}

			Vec3 leftBottomPoint3D = Vec3(leftBottomPoint.x, leftBottomPoint.y, 0.0f);
			Vec3 rightBottomPoint3D = Vec3(rightBottomPoint.x, rightBottomPoint.y, 0.0f);
			Vec3 leftTopPoint3D = Vec3(leftTopPoint.x, leftTopPoint.y, 0.0f);
			Vec3 rightTopPoint3D = Vec3(rightTopPoint.x, rightTopPoint.y, 0.0f);

			AABB2 uvBoundsOfSpriteDef = spriteOfLetter.GetUVs();

			Vec2 leftBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 0.0f));
			Vec2 rightBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 0.0f));
			Vec2 leftTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 1.0f));
			Vec2 rightTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 1.0f));

			Vertex_PCU leftBottomPCU = Vertex_PCU(leftBottomPoint3D, tint, leftBottomUVs);
			Vertex_PCU rightBottomPCU = Vertex_PCU(rightBottomPoint3D, tint, rightBottomUVs);
			Vertex_PCU leftTopPCU = Vertex_PCU(leftTopPoint3D, tint, leftTopUVs);
			Vertex_PCU rightTopPCU = Vertex_PCU(rightTopPoint3D, tint, rightTopUVs);

			vertexArray.push_back(leftBottomPCU);
			vertexArray.push_back(rightBottomPCU);
			vertexArray.push_back(rightTopPCU);

			vertexArray.push_back(leftBottomPCU);
			vertexArray.push_back(rightTopPCU);
			vertexArray.push_back(leftTopPCU);

			maxLettersToDraw -= 1;
		}
	}

}

void BitmapFont::AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& vertexArray, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, Vec2 const& alignment /*= Vec2(.5f, .5f)*/, TextBoxMode mode /*= TextBoxMode::SHRINK_TO_FIT*/, int maxGlyphsToDraw /*= 99999999*/)
{
	UNUSED(maxGlyphsToDraw);
	UNUSED(mode);
	UNUSED(alignment);

	int vertsNum = (int)text.size() * 6;
	vertexArray.reserve(vertsNum);

	float cellWidth = cellHeight * cellAspect;

	Vec3 currentTextPosition = Vec3(0.0f, 0.0f, 0.0f);

	for (int charIndex = 0; charIndex < text.size(); ++charIndex)
	{
		int asciiValue = static_cast<int>(text[charIndex]);
		const SpriteDefinition& spriteOfLetter = m_fontGlyphsSpriteSheet->GetSpriteDef(asciiValue);

		Vec3 leftBottomPoint = currentTextPosition + Vec3(0.0f, 0.0f, 0.0f);
		Vec3 rightBottomPoint = currentTextPosition + Vec3(cellWidth, 0.0f, 0.0f);
		Vec3 leftTopPoint = currentTextPosition + Vec3(0.0f,cellHeight,0.0f);
		Vec3 rightTopPoint = currentTextPosition + Vec3(cellWidth, cellHeight, 0.0f);

		AABB2 uvBoundsOfSpriteDef = spriteOfLetter.GetUVs();

		Vec2 leftBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 0.0f));
		Vec2 rightBottomUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 0.0f));
		Vec2 leftTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(0.0f, 1.0f));
		Vec2 rightTopUVs = uvBoundsOfSpriteDef.GetPointAtUV(Vec2(1.0f, 1.0f));

		Vertex_PCU leftBottomPCU = Vertex_PCU(Vec3(leftBottomPoint.x, leftBottomPoint.y, 0.0f), tint, leftBottomUVs);
		Vertex_PCU rightBottomPCU = Vertex_PCU(Vec3(rightBottomPoint.x, rightBottomPoint.y, 0.0f), tint, rightBottomUVs);
		Vertex_PCU leftTopPCU = Vertex_PCU(Vec3(leftTopPoint.x, leftTopPoint.y, 0.0f), tint, leftTopUVs);
		Vertex_PCU rightTopPCU = Vertex_PCU(Vec3(rightTopPoint.x, rightTopPoint.y, 0.0f), tint, rightTopUVs);

		vertexArray.push_back(leftBottomPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(leftTopPCU);

		vertexArray.push_back(leftTopPCU);
		vertexArray.push_back(rightBottomPCU);
		vertexArray.push_back(rightTopPCU);

		currentTextPosition += Vec3(cellWidth, 0.0f, 0.0f);
	}
	AABB2 bounds = GetVertexBounds2D(vertexArray);
	Mat44 tranformMatrix;
	tranformMatrix.AppendZRotation(90.0f);
	tranformMatrix.AppendXRotation(90.0f);
	tranformMatrix.AppendTranslation2D(-bounds.GetCenter());
	TransformVertexArray3D(vertexArray, tranformMatrix);
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect /*= 1.f*/)
{
	float singleWidth = cellHeight * cellAspect;

	return singleWidth * text.size();
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	return 1.0f;
}
