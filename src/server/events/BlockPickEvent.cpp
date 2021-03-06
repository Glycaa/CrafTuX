#include "BlockPickEvent.h"
#include "server/Server.h"

BlockPickEvent::BlockPickEvent(const BlockPosition& position, Player* player) : PlayerEvent(player), m_blockPosition(position)
{
}

void BlockPickEvent::perform(Server& server) const
{
	m_player->giveOne(server.world().block(m_blockPosition)->id());
	server.world().block(m_blockPosition)->setId(0);
	// Make the chunk and chunks arround redrawed
	server.world().chunk(m_blockPosition)->makeDirty();
	server.world().chunk(m_blockPosition)->makeSurroundingChunksDirty();
}

QByteArray BlockPickEvent::serialize() const
{
	// TODO
	QByteArray byteArray;

	return byteArray;
}
