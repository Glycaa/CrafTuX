#include "LocalServerConnector.h"

#include <QDebug>

LocalServerConnector::LocalServerConnector()
{
	QThread* thread = new QThread(this);
	m_server = new LocalServer(thread);
	m_me = new Me(&world());
	connect(&world(), SIGNAL(chunkLoaded(ChunkPostition)), this, SLOT(onChunkLoaded(ChunkPostition)));
	qDebug() << "Initialized" << metaObject()->className();
	thread->start();

	// For this LocalServerConnector, we simply send the signals to the embedded server...
	connect(this, SIGNAL(wantPickBlock(BlockPosition)), m_server, SLOT(pickBlock(BlockPosition)));
	connect(this, SIGNAL(wantUseBlock(BlockPosition)), m_server, SLOT(useBlock(BlockPosition)));
}

void LocalServerConnector::onChunkLoaded(ChunkPostition position)
{
	if(world().chunk(position) == world().chunk(me()->v_position)) // If this is the chunk the player is standing on
	{
		// Then load some chunks arround
		const int chunksLoadPseudoRadius = 2;
		for(int x = - chunksLoadPseudoRadius; x <= chunksLoadPseudoRadius; x++)
		{
			for(int z = - chunksLoadPseudoRadius; z <= chunksLoadPseudoRadius; z++)
			{
				world().loadChunk(ChunkPostition(position.first + x, position.second + z));
			}
		}
	}
}

