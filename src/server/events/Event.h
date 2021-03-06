#ifndef EVENT_H
#define EVENT_H

const int MAX_EVENTID = 50000;

#include <QByteArray>

/*! An abstract class for events */
class Event
{
public:
	Event();

	/*! Returns an unique ID per event type. This is a mean to recognize serialized events */
	virtual int id() = 0;

	/*! Serialize the event, for example to be sent trought the network. */
	virtual QByteArray serialize() const = 0;
};

#endif // EVENT_H
