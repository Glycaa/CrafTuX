﻿#include "WorldBlocks.h"

#include <stdlib.h> // NULL
#include <cmath> // floor

WorldBlocks::WorldBlocks(const int WORLD_SIZE_X, const int WORLD_SIZE_Y, const int WORLD_SIZE_Z, const int SEA_LEVEL) : i_SIZE_X(WORLD_SIZE_X), i_SIZE_Y(WORLD_SIZE_Y), i_SIZE_Z(WORLD_SIZE_Z), i_SEA_LEVEL(SEA_LEVEL)
{
	int size = i_SIZE_X * i_SIZE_Y * i_SIZE_Z;
	p_BlocksInfo = new BlockInfo[size];
}

void WorldBlocks::generate(int seed)
{
	qDebug("Génération du monde...");
	qDebug("Taille demandée : (x=%d; y=%d; z=%d) sea_level : %d", i_SIZE_X, i_SIZE_Y, i_SIZE_Z, i_SEA_LEVEL);

	// de la surface jusq'au dessus
	for(int j = (i_SIZE_Y - i_SEA_LEVEL); j < (i_SIZE_Y); j++)
	{
		for(int i = 0; i < i_SIZE_X; i++)
		{
			for(int k = 0; k < i_SIZE_Z; k++)
			{
				// On ne met que du vide
				block(i, j, k)->setValue(0);
			}
		}
	}

	// de la surface jusq'en dessous
	for(int j = (i_SIZE_Y - i_SEA_LEVEL); j >= 0; j--)
	{
		for(int i = 0; i < i_SIZE_X; i++)
		{
			for(int k = 0; k < i_SIZE_Z; k++)
			{
				// Il y a une grosse proba d'avoir de la roche, puis un peu de terre, puis rarerent un trou
				int random = qrand() % 9;
				// 0, 1, 2, 3, 4, 5 -> Roche
				if(random <= 5) block(i, j, k)->setValue(1);
				// 6, 7 -> Terre
				if(random == 6 || random == 7) block(i, j, k)->setValue(2);
				// 8 -> Vide
				if(random == 8) block(i, j, k)->setValue(0);
			}
		}
	}
	qDebug("Génération du monde terminée !");
	qDebug("Poids brut total : %ld Kio", (i_SIZE_X * i_SIZE_Y * i_SIZE_Z * sizeof(BlockInfo)) >> 10);
}

BlockInfo* WorldBlocks::blockBelow(preal f_x, preal f_y, preal f_z) {
	int i_bx, i_by, i_bz;
	WorldBlocks::prealToInt(f_x, f_y, f_z, i_bx, i_by, i_bz);
	i_by--;
	return block(i_bx, i_by, i_bz);
}

void WorldBlocks::prealToInt(preal f_x, preal f_y, preal f_z, int& i_x, int& i_y, int& i_z) {
	i_x = floor(f_x);
	i_y = floor(f_y);
	i_z = floor(f_z);
}