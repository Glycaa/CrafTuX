#include "server/events/ChunkConnectEvent.h"
#include "server/events/BlockPickEvent.h"
#include "server/events/BlockUseEvent.h"
#include "server/events/SlotSelectEvent.h"
#include "ServerConnector.h"

#include <QDebug>

ServerConnector::ServerConnector(QObject *parent) : QObject(parent)
{
	qDebug() << "Initialized" << metaObject()->className();
}

World& ServerConnector::world()
{
	Q_ASSERT(1!=1);
	return *(World*)0x123456789; // TODO WARNING : we can't access a world from a ServerConnector, only with its children.
}

void ServerConnector::takeEvent(const ClientEvent* event)
{

}

void ServerConnector::loadAndPruneChunks()
{
	ChunkPosition currentPosition = world().chunkPosition(me()->v_position.x, me()->v_position.z);
	QList<ChunkPosition> wantedChunks; // The chunks we still want to be active

	// Create a list of the wanted chunks
	for(int x = - i_viewDistance; x < i_viewDistance+1; ++x)
	{
		for(int z = - i_viewDistance; z < i_viewDistance+1; ++z)
		{
			ChunkPosition position = ChunkPosition(currentPosition.first + x, currentPosition.second + z);
			wantedChunks.push_back(position);
		}
	}

	// Let's see if we have to prune unwanted ones...
	for (int i = 0; i < m_loadedChunks.size(); ++i) {
		ChunkPosition processingChunk = m_loadedChunks.at(i);

		// If the chunk is wanted
		if(wantedChunks.contains(processingChunk)) {
			// delete it from the wanted ones
			wantedChunks.removeOne(processingChunk);
		}
		else {
			// The chunk is unwanted, get rid of it
			ChunkConnectEvent* event = new ChunkConnectEvent(processingChunk, ChunkConnectEvent::ChunkConnection_Disconnect);
			emit postEvent(event);
			// Delete it from the loaded chunks list
			m_loadedChunks.removeAt(i); i--;
		}
	}

	// Now we load the chunks that were not in the loaded chunks
	for (int i = 0; i < wantedChunks.size(); ++i) {
		ChunkPosition processingChunk = wantedChunks.at(i);
		ChunkConnectEvent* event = new ChunkConnectEvent(processingChunk, ChunkConnectEvent::ChunkConnection_Connect);
		emit postEvent(event);
		m_loadedChunks.push_back(processingChunk);
	}
}

void ServerConnector::pickBlock()
{
	BlockPickEvent* event = new BlockPickEvent(me()->pointedBlock(), me());
	emit postEvent(event);
}

void ServerConnector::useBlock()
{
	BlockUseEvent* event = new BlockUseEvent(me()->pointedFreeBlock(), me());
	emit postEvent(event);
}

void ServerConnector::selectSlot(const int selectedSlot)
{
	// Check that the slot id we demand is valid
	int newSelectedSlot = selectedSlot;
	if(newSelectedSlot < 0) {
		newSelectedSlot = INVENTORY_SIZE - 1;
	}
	if(newSelectedSlot >= (int)INVENTORY_SIZE) {
		newSelectedSlot = 0;
	}
	SlotSelectEvent* event = new SlotSelectEvent(newSelectedSlot, me());
	emit postEvent(event);
}

void ServerConnector::setViewDistance(const int distance)
{
	i_viewDistance = distance;
}
