#ifndef GLGAME_H
#define GLGAME_H

#include "GLRect.h"
#include "GLPoint.h"
#include "GLRenderer.h"
#include "GLImage.h"
#include "GLSounds.h"
#include "GLCursor.h"
#include "GLUtils.h"

#define kNumLightningPts 8
#define kMaxEnemies 8

struct playerType {
	GLRect dest, wasDest, wrap;
	int h, v;
	int wasH, wasV;
	int hVel, vVel;
	int srcNum, mode;
	int frame;
	bool facingRight, flapping;
	bool walking, wrapping;
	bool clutched;
};

struct enemyType {
	GLRect dest, wasDest;
	int h, v;
	int wasH, wasV;
	int hVel, vVel;
	int srcNum, mode;
	int kind, frame;
	int heightSmell, targetAlt;
	int flapImpulse, pass;
	int maxHVel, maxVVel;
	bool facingRight;
} ;

enum GLGameKey {
    kGLGameKeyNone = 0,
    
    kGLGameKeySpacebar = 1,
    kGLGameKeyDownArrow = 2,
    kGLGameKeyLeftArrow = 4,
    kGLGameKeyRightArrow = 8,
    kGLGameKeyA = 16,
    kGLGameKeyS = 32,
    kGLGameKeyColon = 64,
    kGLGameKeyQuote = 128,
};

enum GLGameEvent {
    kGLGameStarted,
    kGLGameEnded,
};

typedef void (*GLGameCallback)(GLGameEvent event, void *context);

class GLGame {
public:
    GLGame(GLGameCallback callback, void *context);
    ~GLGame();
    
    GLRenderer* renderer();
    
    void run();
    
    void handleMouseDownEvent(const GLPoint& point);
    void handleKeyDownEvent(GLGameKey key);
    void handleKeyUpEvent(GLGameKey key);
    
    void newGame();
    void endGame();
    
private:
    GLGameCallback callback_;
    void *callbackContext_;
    
    GLRenderer *renderer_;
    GLCursor cursor;
    GLUtils utils;
    
    double now;
    void loadImages();
    bool playing, evenFrame, flapKeyDown;
    
    void update();
    void drawFrame();
    
    GLImage bgImg;
    void drawBackground() const;

    GLImage torchesImg;
    GLRect flameDestRects[2], flameRects[4];
    void drawTorches() const;

    void drawLightning();
    void generateLightning(short h, short v);
    void strikeLightning();
    void doLightning(const GLPoint& point);
    GLPoint leftLightningPts[kNumLightningPts], rightLightningPts[kNumLightningPts];
    GLPoint mousePoint;
    int lightningCount;
    double lastLightningStrike;
    GLPoint lightningPoint;
    int newGameLightning;
    double lastNewGameLightning;
    GLRect obeliskRects[4];
    GLImage obelisksImg;
    bool flashObelisks;
    void drawObelisks() const;
    
    int numLedges, levelOn, livesLeft;
    
    playerType thePlayer;
    GLRect playerRects[11];
    void resetPlayer(bool initialPlace);
    void offAMortal();
    GLImage playerImg;
    GLImage playerIdleImg;
    void drawPlayer() const;
    void movePlayer();
    void handlePlayerIdle();
    void handlePlayerWalking();
    void handlePlayerFlying();
    void handlePlayerSinking();
    void handlePlayerFalling();
    void handlePlayerBones();
    void setAndCheckPlayerDest();
    void checkTouchDownCollision();
    void checkPlatformCollision();
    void setUpLevel();
    void checkLavaRoofCollision();
    void checkPlayerWrapAround();
    void keepPlayerOnPlatform();
    
    void getPlayerInput();
    int theKeys;
    GLRect platformRects[6], touchDownRects[6], enemyRects[24];
    
    GLRect platformCopyRects[9];
    void drawPlatforms() const;
    GLImage platformImg;
    
    GLSounds sounds;
    
    long theScore;
    GLImage numbersImg;
    GLRect numbersSrc[11], numbersDest[11];
    void drawLivesNumbers() const;
    void drawScoreNumbers() const;
    void drawLevelNumbers() const;
    void addToScore(int value);
    
    typedef struct {
        GLRect dest;
        int mode;
    } handInfo;
    GLImage handImg;
    handInfo theHand;
    GLRect grabZone;
    GLRect handRects[2];
    void initHandLocation();
    void handleHand();
    
    int countDownTimer;
    void handleCountDownTimer();
    
    int numEnemies;
    int numEnemiesThisLevel;
    int deadEnemies;
    int numOwls;
    int spawnedEnemies;
    enemyType theEnemies[kMaxEnemies];
    GLRect enemyInitRects[5];
    GLRect eggSrcRect;
    bool doEnemyFlapSound;
	bool doEnemyScrapeSound;
    GLImage enemyFly;
    GLImage enemyWalk;
    GLImage egg;
    void moveEnemies();
    void checkEnemyWrapAround(int who) const;
    void drawHand() const;
    void drawEnemies() const;
    void generateEnemies();
    bool setEnemyInitialLocation(GLRect *theRect);
    void initEnemy(short i, bool reincarnated);
    void setEnemyAttributes(int i);
    int assignNewAltitude();
    void checkEnemyPlatformHit(int h);
    void checkEnemyRoofCollision(int i);
    void handleIdleEnemies(int i);
    void handleFlyingEnemies(int i);
    void handleWalkingEnemy(int i);
    void handleSpawningEnemy(int i);
    void handleFallingEnemy(int i);
    void handleEggEnemy(int i);
    void resolveEnemyPlayerHit(int i);
    void checkPlayerEnemyCollision();
};

#endif
