﻿#include "glextensions.h" // On inclue les extensions OpenGL
#include "MainWindow.h"
#include "TextureManager.h"
#include "Utils.h"
#include "PhysicEngine.h"
#include <cstdlib> // rand()

const int LightPos[4] = {0,100,0,1};
GLfloat AmbientColor[] = {(255.0f / 255.0f) -0.7f, (255.0f / 255.0f) -0.7f, (235.0f / 255.0f) -0.7f, 1.0f};

GLfloat diffuse[] = {1.0f, 1.0f, 1.0f , 1.0f};
GLfloat position[] = { 0.0f, WORLD_SIZE_Y, 0.0f, 0.5f };


// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// vertex coords array
GLfloat vertices[] = {1,1,1,  -1,1,1,  -1,-1,1,  1,-1,1,        // v0-v1-v2-v3
                      1,1,1,  1,-1,1,  1,-1,-1,  1,1,-1,        // v0-v3-v4-v5
                      1,1,1,  1,1,-1,  -1,1,-1,  -1,1,1,        // v0-v5-v6-v1
                      -1,1,1,  -1,1,-1,  -1,-1,-1,  -1,-1,1,    // v1-v6-v7-v2
                      -1,-1,-1,  1,-1,-1,  1,-1,1,  -1,-1,1,    // v7-v4-v3-v2
                      1,-1,-1,  -1,-1,-1,  -1,1,-1,  1,1,-1};   // v4-v7-v6-v5

// normal array
GLfloat normals[] = {0,0,1,  0,0,1,  0,0,1,  0,0,1,             // v0-v1-v2-v3
                     1,0,0,  1,0,0,  1,0,0, 1,0,0,              // v0-v3-v4-v5
                     0,1,0,  0,1,0,  0,1,0, 0,1,0,              // v0-v5-v6-v1
                     -1,0,0,  -1,0,0, -1,0,0,  -1,0,0,          // v1-v6-v7-v2
                     0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,         // v7-v4-v3-v2
                     0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1};        // v4-v7-v6-v5

// color array
GLfloat colors[] = {1,1,1,  1,1,0,  1,0,0,  1,0,1,              // v0-v1-v2-v3
                    1,1,1,  1,0,1,  0,0,1,  0,1,1,              // v0-v3-v4-v5
                    1,1,1,  0,1,1,  0,1,0,  1,1,0,              // v0-v5-v6-v1
                    1,1,0,  0,1,0,  0,0,0,  1,0,0,              // v1-v6-v7-v2
                    0,0,0,  0,0,1,  1,0,1,  1,0,0,              // v7-v4-v3-v2
                    0,0,1,  0,0,0,  0,1,0,  0,1,1};             // v4-v7-v6-v5

// index array of vertex array for glDrawElements()
// Notice the indices are listed straight from beginning to end as exactly
// same order of vertex array without hopping, because of different normals at
// a shared vertex. For this case, glDrawArrays() and glDrawElements() have no
// difference.
GLubyte indices[] = {0,1,2,3,
                     4,5,6,7,
                     8,9,10,11,
                     12,13,14,15,
                     16,17,18,19,
                     20,21,22,23};

MainWindow::MainWindow(WorldBlocks* worldBlocks) : GLWidget(), b_lightingEnabled(false), b_textureEnabled(false), b_infosEnabled(false), b_nowPlaying(true), f_cameraAngle(45.0f), m_worldBlocks(worldBlocks), m_originalCursor(this->cursor())
{
    setWindowTitle(tr("Programme de test tournant sous OpenGL, développé par Glyca"));
    setMouseTracking(true);

    i_winwidth = this->width();
    i_winheight = this->height();

    t_secondTimer = new QTimer(this);
    t_secondTimer->setInterval(1000);
    connect(t_secondTimer, SIGNAL(timeout()), this, SLOT(secondTimerProcess()));
    t_secondTimer->start();

    po_character = PhysicEngine->createPhysicObject();
    po_character->v3_position.y = 100.0f;

    // Configuration de la caméra
    // Now set up our max values for the camera
    glc_camera.m_MaxForwardVelocity = 3.0f;
    glc_camera.m_MaxPitchRate = 5.0f;
    glc_camera.m_MaxHeadingRate = 5.0f;
    glc_camera.m_PitchDegrees = 0.0f;
    glc_camera.m_HeadingDegrees = 180.0f;
    glc_camera.m_Position.x = WORLD_SIZE_X/2;
    glc_camera.m_Position.y = WORLD_SIZE_Y - WORLD_SEA_LEVEL + 1;
    glc_camera.m_Position.z = WORLD_SIZE_Z/2;

    QCursor::setPos(i_winwidth << 1, i_winheight << 1); // /2
}

void MainWindow::initializeGL()
{
    qDebug() << "L'initialisation d'OpenGL a reussi";
    qDebug("OpenGL>Window_version : %d.%d", format().majorVersion(), format().minorVersion());
    qDebug() << "OpenGL>Vendor :" << (char*)glGetString(GL_VENDOR);
    qDebug() << "OpenGL>Renderer :" << (char*)glGetString(GL_RENDERER);
    qDebug() << "OpenGL>Version :" << (char*)glGetString(GL_VERSION);

    GLint max_lights; glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
    qDebug() << "OpenGL>Max Lights :" << max_lights;

    // Extensions OpenGL
    getGLExtensionFunctions().resolve(this->context());
    qDebug() << "OpenGL>1.5_support :" << getGLExtensionFunctions().openGL15Supported();
    qDebug() << "OpenGL>Has_FBO :" << getGLExtensionFunctions().fboSupported();

    glShadeModel(GL_SMOOTH);
    glClearColor(138.0f / 255.0f, 219.0f / 255.0f, 206.0f / 255.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST); // test de profondeur
    glDepthFunc(GL_LEQUAL);  // Fontion du test de profondeur
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE); // Optimisation
    glEnable(GL_LINE_SMOOTH); // Dessine de belles lignes

    // On indique la couleur de la lumière ambiante
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientColor);
}

void MainWindow::resizeGL(int width, int height)
{
    i_winwidth = width; i_winheight = height;
    resizeGLreally();
}

void MainWindow::resizeGLreally()
{
    if(i_winheight == 0)
	i_winheight = 1;
    glViewport(0, 0, i_winwidth, i_winheight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(f_cameraAngle, (GLfloat)i_winwidth/(GLfloat)i_winheight, 0.1f, 2000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MainWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calcul de la caméra
    glc_camera.SetPrespective();

    // Rendu des blocs
    renderBlocks();

    // Rendu des axes Ox, Oy, Oz
    Utils->fastAxes();

    PhysicEngine->processMoves();

    glColor3f(1.0f, 1.0f ,1.0f); // On écrit les infos en blanc

    if(b_infosEnabled)
    {
    int offset = 10; glLoadIdentity();

	renderText(5, offset, "------ Informations (F3) ------");

	if(!b_nowPlaying)
	{
	    offset += 30;
	    renderText(5, offset, "LE JEU EST EN PAUSE  (ECHAP)");
	    offset += 15;
	    renderText(5, offset, "Quitter en appuyant sur (C)");
	    offset += 30;
	}

	offset += 20;
	renderText(5, offset, "Caméra : (SOURIS ET 8,4,6,2)");
	offset += 20;
	renderText(5, offset, "Angle de vue (ZOOM) = " + ((QVariant)f_cameraAngle).toString() + "° (PAGE_UP/PAGE_DOWN)");

	offset += 30;
	renderText(5, offset, "Position du joueur : (" + ((QVariant)glc_camera.m_Position.x).toString() + ";" + ((QVariant)glc_camera.m_Position.y).toString() + ";" + ((QVariant)glc_camera.m_Position.z).toString() + ") (UP/DOWN)");
    offset += 20;
    renderText(5, offset, "Vecteur direction : (" + ((QVariant)glc_camera.m_DirectionVector.i).toString() + ";" + ((QVariant)glc_camera.m_DirectionVector.j).toString() + ";" + ((QVariant)glc_camera.m_DirectionVector.k).toString() + ")");

	offset += 30;
	renderText(5, offset, "PhysicObject de masse " + ((QVariant)po_character->getMass()).toString() + "kg :");
	offset += 20;
	renderText(5, offset, "Position = (" + ((QVariant)po_character->v3_position.x).toString() + ";" + ((QVariant)po_character->v3_position.y).toString() + ";" + ((QVariant)po_character->v3_position.z).toString() + ")");
	offset += 20;
	renderText(5, offset, "Vitesse = (" + ((QVariant)po_character->v3_velocity.x).toString() + ";" + ((QVariant)po_character->v3_velocity.y).toString() + ";" + ((QVariant)po_character->v3_velocity.z).toString() + ")");
	offset += 20;
	renderText(5, offset, "Accélération = (" + ((QVariant)po_character->v3_acceleration.x).toString() + ";" + ((QVariant)po_character->v3_acceleration.y).toString() + ";" + ((QVariant)po_character->v3_acceleration.z).toString() + ")");
	offset += 20;
	renderText(5, offset, "Poussée de 1N selon Ox en appuyant sur (F)");

	offset += 30;
	renderText(5, offset, "[" + (b_lightingEnabled?QString("X"):"  ") + "] Eclairage activé (L)");
	offset += 20;
    renderText(5, offset, "[" + (b_textureEnabled?QString("X"):"  ") + "] Textures activées (T)");
    }

    swapBuffers();

    setWindowTitle(((QVariant)i_lastFpsCount).toString() + "FPS " + tr("Programme de test tournant sous OpenGL, développé par Glyca"));

    i_fpsCount++;
}

void MainWindow::paintEventprout(QPaintEvent *paintEvent)
{
    // On dessine d'abord la 3D :
    qglClearColor(QColor::fromCmykF(0.40, 0.0, 1.0, 0.0));
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //resizeGLreally();
    paintGL();

/*
    // Puis on dessine la 2D :
    QPainter painter(this);
    QString text = "Click and drag with the left mouse button "
	    "to rotate the Qt logo.";
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
				      Qt::AlignCenter | Qt::TextWordWrap, text);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.fillRect(QRect(0, 0, width(), rect.height() + 2*border),
		     QColor(0, 0, 0, 127));
    painter.setPen(Qt::white);
    painter.fillRect(QRect(0, 0, width(), rect.height() + 2*border),
		     QColor(0, 0, 0, 127));
    painter.drawText((width() - rect.width())/2, border,
		     rect.width(), rect.height(),
		     Qt::AlignCenter | Qt::TextWordWrap, text);
    painter.end();*/
}


void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
    GLWidget::keyPressEvent(keyEvent);

    if(b_nowPlaying)
    {
	if(keyEvent->key() == Qt::Key_Up)
	{
	    glc_camera.m_ForwardVelocity = 0.3f;
	}
	else if(keyEvent->key() == Qt::Key_Down)
	{
	    glc_camera.m_ForwardVelocity = -0.3f;
	}

	if(keyEvent->key() == Qt::Key_Right)
	{
	    glc_camera.m_RightVelocity = 0.3f;
	}
	else if(keyEvent->key() == Qt::Key_Left)
	{
	    glc_camera.m_RightVelocity = -0.3f;
	}

	if(keyEvent->key() == Qt::Key_8) glc_camera.ChangePitch(5.0f);
	if(keyEvent->key() == Qt::Key_2) glc_camera.ChangePitch(-5.0f);
	if(keyEvent->key() == Qt::Key_4) glc_camera.ChangeHeading(-5.0f);
	if(keyEvent->key() == Qt::Key_6) glc_camera.ChangeHeading(5.0f);
    }

    switch(keyEvent->key())
    {
    case Qt::Key_Escape:
	b_nowPlaying = !b_nowPlaying;
	if(b_nowPlaying) setMouseTracking(true);
	else{
	    setMouseTracking(false);
	    this->setCursor(m_originalCursor); }
	break;

    case Qt::Key_E:
	qDebug() << "OpenGL>Extensions :" << (QString((char*)glGetString(GL_EXTENSIONS))).toLatin1();
	break;

    case Qt::Key_L:
	toggleLighting();
	break;

    case Qt::Key_F:
	po_character->applyForcev(Vector3(1.0, 0, 0));
	break;

    case Qt::Key_T:
	toggleTexture();
	break;

    case Qt::Key_A:
	b_infosEnabled = !b_infosEnabled;
	toggleLighting();
	toggleTexture();
	break;

    case Qt::Key_PageUp:
	f_cameraAngle += 1.0f;
	resizeGLreally();
	break;

    case Qt::Key_PageDown:
	f_cameraAngle -= 1.0f;
	resizeGLreally();
	break;

    case Qt::Key_0:
	f_cameraAngle = 45.0f;
	resizeGLreally();
	break;

    case Qt::Key_F3:
	b_infosEnabled = !b_infosEnabled;
	qDebug("Infos actives");
	break;
    }
}

// Appelée lorsqu'une touch $e est relachée
void MainWindow::keyReleaseEvent(QKeyEvent *keyEvent)
{
    GLWidget::keyReleaseEvent(keyEvent);

    if((keyEvent->key() == Qt::Key_Up) || (keyEvent->key() == Qt::Key_Down))
    {
	glc_camera.m_ForwardVelocity = 0.0f; // On stoppe le perso
    }
    else if((keyEvent->key() == Qt::Key_Left) || (keyEvent->key() == Qt::Key_Right))
    {
	glc_camera.m_RightVelocity = 0.0f; // On stoppe le perso
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    GLfloat DeltaMouse; int MouseX, MouseY;

    MouseX = mouseEvent->x();
    MouseY = mouseEvent->y();

    int CenterX = i_winwidth / 2;
    int CenterY = i_winheight / 2;

    if(MouseX < CenterX)
    {
	DeltaMouse = GLfloat(CenterX - MouseX);
	glc_camera.ChangeHeading(-0.2f * DeltaMouse);
    }
    else if(MouseX > CenterX)
    {
	DeltaMouse = GLfloat(MouseX - CenterX);
	glc_camera.ChangeHeading(0.2f * DeltaMouse);
    }

    if(MouseY < CenterY)
    {
	DeltaMouse = GLfloat(CenterY - MouseY);
	glc_camera.ChangePitch(-0.2f * DeltaMouse);
    }
    else if(MouseY > CenterY)
    {
	DeltaMouse = GLfloat(MouseY - CenterY);
	glc_camera.ChangePitch(0.2f * DeltaMouse);
    }

    QCursor newCursor(this->cursor());
    newCursor.setPos(mapToGlobal(QPoint(CenterX, CenterY)));
    newCursor.setShape(Qt::BlankCursor);
    this->setCursor(newCursor);
}

void MainWindow::toggleLighting()
{
    b_lightingEnabled = !b_lightingEnabled;

    if(b_lightingEnabled)
    {
	glEnable(GL_LIGHTING); 	// Active l'éclairage
	glEnable(GL_LIGHT0); 	// Allume la lumière n°1
	glEnable(GL_COLOR_MATERIAL);
	// Lumière spéculaire
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	qDebug("Eclairage active");
    }
    else
    {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	qDebug("Eclairage desactive");
    }
}

void MainWindow::toggleTexture()
{
    b_textureEnabled = !b_textureEnabled;

    if(b_textureEnabled)
    {
	glEnable(GL_TEXTURE_2D);
	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glt_Dirt = TextureManager->loadTexture("Soil.png");
        glt_Rock = TextureManager->loadTexture("Rock.png");
	qDebug("Texture activees. Rock:%d ; Dirt:%d", glt_Rock, glt_Dirt);
    }
    else
    {
	deleteTexture(glt_Dirt);
	deleteTexture(glt_Rock);
	glDisable(GL_TEXTURE_2D);
	qDebug("Texture deactivees.");
    }
}

void MainWindow::secondTimerProcess()
{
    i_lastFpsCount = i_fpsCount;
    i_fpsCount = 0;
}

void MainWindow::renderBlocks()
{
	int i_worldSizeX = m_worldBlocks->getSizeX();
	int i_worldSizeY = m_worldBlocks->getSizeY();
	int i_worldSizeZ = m_worldBlocks->getSizeZ();

    // Rock
	if(b_textureEnabled) glBindTexture(GL_TEXTURE_2D, glt_Rock); // Choix de la texture
	for(int i = 0; i < i_worldSizeX; i++)
    {
	for(int j = 0; j < i_worldSizeY; j++)
	{
		for(int k = 0; k < i_worldSizeZ; k++)
	    {
		if(m_worldBlocks->block(i, j, k)->getValue() == 1)
		{
		    if(!b_textureEnabled)
		    {
			glColor3f(0.68f, 0.68f, 0.68f);
		    }
		    glTranslatef(i, j, k);
		    Utils->fastCube();
		    glTranslatef(-i, -j, -k);
		}
	    }
	}
    }

    // Dirt
    if(b_textureEnabled) glBindTexture(GL_TEXTURE_2D, glt_Dirt); // Choix de la texture
	for(int i = 0; i < i_worldSizeX; i++)
    {
	for(int j = 0; j < i_worldSizeY; j++)
	{
		for(int k = 0; k < i_worldSizeZ; k++)
	    {
		if(m_worldBlocks->block(i, j, k)->getValue() == 2)
		{
		    if(!b_textureEnabled)
		    {
			glColor3f(0.488f, 0.296f, 0.078f);
		    }
		    glTranslatef(i, j, k);
		    Utils->fastCube();
		    glTranslatef(-i, -j, -k);
		}
	    }
	}
    }

    // Le petit physicObject !
    if(b_textureEnabled) glBindTexture(GL_TEXTURE_2D, 0); // Choix de la texture

	glColor3f(0.078f, 0.296f, 0.488f);

    glTranslatef(po_character->v3_position.x, po_character->v3_position.y, po_character->v3_position.z);

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    //glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices); les indices sont inutiles à cause des normales qui changent à chaque fois
    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glTranslatef(-po_character->v3_position.x, -po_character->v3_position.y, -po_character->v3_position.z);
}



