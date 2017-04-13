#include <base/system.h>
#include <base/math.h>
#include <base/vmath.h>
#include <engine/shared/config.h>

#include "mapgen.h"
#include <game/server/gamecontext.h>
#include <game/layers.h>
#include <game/mapitems.h>

CMapGen::CMapGen()
{
	m_pLayers = 0x0;
	m_pCollision = 0x0;
	m_pNoise = 0x0;
}
CMapGen::~CMapGen()
{
	if (m_pNoise)
		delete m_pNoise;
}

void CMapGen::Init(CLayers *pLayers, CCollision *pCollision)
{
	m_pLayers = pLayers;
	m_pCollision = pCollision;
}

void CMapGen::FillMap(int Seed)
{
	dbg_msg("mapgen", "started map generation with seed=%d", Seed);

	if (m_pNoise)
		delete m_pNoise;
	m_pNoise = new CPerlinOctave(7, Seed);

	int64 ProcessTime = 0;
	int64 TotalTime = time_get();

	int MineTeeLayerSize = m_pLayers->GameLayer()->m_Width*m_pLayers->GameLayer()->m_Height;

	// clear map, but keep background, envelopes etc
	ProcessTime = time_get();
	for(int i = 0; i < MineTeeLayerSize; i++)
	{
		int x = i%m_pLayers->GameLayer()->m_Width;
		ivec2 TilePos(x, (i-x)/m_pLayers->GameLayer()->m_Width);
		
		// clear the different layers
		ModifTile(TilePos, m_pLayers->GetGameLayerIndex(), 1);
		ModifTile(TilePos, m_pLayers->GetBackgroundLayerIndex(), 13);
		ModifTile(TilePos, m_pLayers->GetForegroundLayerIndex(), 1);
	}
	dbg_msg("mapgen", "map normalized in %.5fs", (float)(time_get()-ProcessTime)/time_freq());


	// tunnels
	ProcessTime = time_get();
	GenerateTunnels(m_pNoise->Perlin()->GetURandom(10,20));
	dbg_msg("mapgen", "tunnels generated in %.5fs", (float)(time_get()-ProcessTime)/time_freq());

	// caves
	ProcessTime = time_get();
	GenerateCaves(0);
	dbg_msg("mapgen", "caves generated in %.5fs", (float)(time_get()-ProcessTime)/time_freq());

	// Performance
	dbg_msg("mapgen", "finalizing map...");
	GenerateSkip();

	dbg_msg("mapgen", "map successfully generated in %.5fs", (float)(time_get()-TotalTime)/time_freq());
}

void CMapGen::GenerateCaves(int FillBlock)
{
	const int CaveLevel = PercOf(CAVE_LEVEL, m_pLayers->GameLayer()->m_Height);

	// cut in the caves with a 2d perlin noise
	const float frequency = 32.0f / (float)m_pLayers->GameLayer()->m_Width;
	for(int x = 0; x < m_pLayers->GameLayer()->m_Width; x++)
	{
		for(int y = CaveLevel; y < m_pLayers->GameLayer()->m_Height; y++)
		{
			float noise = m_pNoise->Noise((float)x * frequency, (float)y * frequency);
	
			if(noise > 0.01f)
			{
				ModifTile(ivec2(x, y), m_pLayers->GetGameLayerIndex(), FillBlock);
				ModifTile(ivec2(x, y), m_pLayers->GetForegroundLayerIndex(), FillBlock);
			}
		}
	}
}

void CMapGen::GenerateTunnels(int Num)
{
	const int TunnelLevel = PercOf(TUNNEL_LEVEL, m_pLayers->GameLayer()->m_Height);

	int TilePosY, Level, Freq, TunnelSize, StartX, EndX;
	for (int r=0; r<Num; ++r)
	{
		Level = TunnelLevel + m_pNoise->Perlin()->GetURandom(0,m_pLayers->GameLayer()->m_Height);
		Freq = m_pNoise->Perlin()->GetURandom(2,10);
		TunnelSize = m_pNoise->Perlin()->GetURandom(2,2);
		StartX = m_pNoise->Perlin()->GetURandom(0,m_pLayers->GameLayer()->m_Width);
		EndX = min(m_pLayers->GameLayer()->m_Width, m_pNoise->Perlin()->GetURandom(0,m_pLayers->GameLayer()->m_Width)+StartX);
		for(int TilePosX = StartX; TilePosX < EndX; TilePosX++)
		{
			float frequency = Freq / (float)m_pLayers->GameLayer()->m_Width;
			TilePosY = m_pNoise->Noise((float)TilePosX * frequency) * (m_pLayers->GameLayer()->m_Height) + Level;
			if (TilePosY < m_pLayers->GameLayer()->m_Height-1)
				for (int i=-TunnelSize; i<=TunnelSize; i++)
				{
					ModifTile(ivec2(TilePosX, TilePosY), m_pLayers->GetGameLayerIndex(), 0);
					ModifTile(ivec2(TilePosX, TilePosY), m_pLayers->GetForegroundLayerIndex(), 0);
				}
		}
	}
}

void CMapGen::GenerateSkip()
{
	for(int g = 0; g < m_pLayers->NumGroups(); g++)
	{
		CMapItemGroup *pGroup = m_pLayers->GetGroup(g);

		for(int l = 0; l < pGroup->m_NumLayers; l++)
		{
			CMapItemLayer *pLayer = m_pLayers->GetLayer(pGroup->m_StartLayer+l);

			if(pLayer->m_Type == LAYERTYPE_TILES)
			{
				CMapItemLayerTilemap *pTmap = (CMapItemLayerTilemap *)pLayer;
				CTile *pTiles = (CTile *)m_pLayers->Map()->GetData(pTmap->m_Data);
				for(int y = 0; y < pTmap->m_Height; y++)
				{
					for(int x = 1; x < pTmap->m_Width;)
					{
						int sx;
						for(sx = 1; x+sx < pTmap->m_Width && sx < 255; sx++)
						{
							if(pTiles[y*pTmap->m_Width+x+sx].m_Index)
								break;
						}

						pTiles[y*pTmap->m_Width+x].m_Skip = sx-1;
						x += sx;
					}
				}
			}
		}
	}
}


inline void CMapGen::ModifTile(ivec2 Pos, int Layer, int Tile)
{
	m_pCollision->ModifTile(Pos, m_pLayers->GetGameGroupIndex(), Layer, Tile, 0, 0);
}
