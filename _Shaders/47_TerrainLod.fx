#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Terrain.fx"

technique11 T0
{
    P_VTP(P0, VS_TerrainLod, HS_TerrainLod, DS_TerrainLod, PS_TerrainLod)
    P_RS_VTP(P1, FillMode_WireFrame, VS_TerrainLod, HS_TerrainLod, DS_TerrainLod, PS_TerrainLod)
}