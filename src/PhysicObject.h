﻿#ifndef PHYSICOBJECT_H
#define PHYSICOBJECT_H

#include <QObject>
#include <QElapsedTimer>
#include "PhysicSize.h"
#include "Vector.h"

class PhysicObject : public QObject
{
    Q_OBJECT
public:
    PhysicObject(preal mass = f_defaultMass);

    // Modifie toutes les conposantes des vecteurs et coordonnées de l'objet
	void processMove(preal f_elapsedTimeSec);

    // Applique un vecteur force avec ces composantes en NEWTON
	void applyForcev(Vector v_force);

	void applyWeightForce();
	void applyFluidFrictionForce();

    inline preal getMass() const {return f_mass;}

public: // Public temporairement

    preal f_mass; // La masse de l'objet en KG

	Vector v_position; // La position de l'objet
	Vector v_velocity, v_acceleration; // Le vecteur vitesse et le vecteur accélération
	Vector v_forces; // Somme des forces appliquées à l'objet

};

#endif // PHYSICOBJECT_H