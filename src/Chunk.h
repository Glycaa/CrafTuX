#ifndef CHUNK_H
#define CHUNK_H

#include <QObject>
#include <QList>
#include <QReadWriteLock>
#include <QPair>

class BlockInfo;
class ChunkDrawer;
class World;

const int CHUNK_X_SIZE = 24;
const int CHUNK_Y_SIZE = 256;
const int CHUNK_Z_SIZE = 24;
const int CHUNK_HEIGHT = CHUNK_Y_SIZE;

typedef QPair<int, int> ChunkPosition;

/*! A chunk of a World containing all BlockInfo */
class Chunk : public QObject
{
	Q_OBJECT
public:
	explicit Chunk(QObject *parent, ChunkPosition position);
	~Chunk();

	/*! This returns the mutex of the Chunk. Be sure to unlock it when needed!! */
	inline QReadWriteLock& rwLock() {return m_rwLock;}

	enum ChunkState {
		ChunkState_Active,
		ChunkState_Idle,
		ChunkState_Void
	};

	inline ChunkPosition position() const {return m_position;}
	int altitude(const int x, const int z);

	/*! Access a block from a chunk
  \warning The coordinates to pass are relative to the chunk, and thus must be inside!
 */
	BlockInfo* block(const int x, const int y, const int z);

	/*! Give the world the chunk belongs to */
	inline World& world() const {return *reinterpret_cast<World*>(parent());}

	/*! Convert coordinates relatives to the chunk into world coordinates */
	void mapToWorld(const int chunkX, const int chunkY, const int chunkZ, int& worldX, int& worldY, int& worldZ) const;

	/*! This will force the chunk to be redrawed */
	void makeDirty();

	void makeSurroundingChunksDirty() const;

	//! Render all blocks of the chunk
	void render3D();

signals:

public slots:
	void activate(); //!< Activate the chunk (it will be drawed)
	void idle(); //!< Make the Chunk enter in an idle state (it will not be drawed)

private:
	QReadWriteLock m_rwLock;
	ChunkState m_state;
	bool b_dirty; //!< If we need to redraw the chunk
	ChunkPosition m_position; //!< The postion of the chunk in chunk unit.
	BlockInfo* p_BlockInfos; //!< A big array of all BlockInfo of the Chunk
	ChunkDrawer* m_chunkDrawer;
};

#endif // CHUNK_H
