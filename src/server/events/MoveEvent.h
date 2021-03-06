#ifndef WALKEVENT_H
#define WALKEVENT_H

#include "ClientEvent.h"
#include "ServerEvent.h"
#include "Entity.h"

const int EVENTID_WALK = 3;

class MoveEvent : public ClientEvent, ServerEvent
{
public:
	MoveEvent(const Entity::WalkDirection direction, Entity* entity);

	inline virtual int id() {return EVENTID_WALK;}

	virtual void perform(Server& server) const;
	virtual QByteArray serialize() const;

protected:
	Entity::WalkDirection m_direction;
	Entity* m_entity;
};

#endif // WALKEVENT_H
