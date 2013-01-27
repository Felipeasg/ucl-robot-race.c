#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <math.h>
#include "common.h"

#include "rangefinders.h"
#include "encoders.h"
#include "bumpers.h"
#include "us.h"

#include "robot.h"
#include "check.h"

#define ACCURATE true
#define INACCURATE false

#define CHECKFRONT 1
#define CHECKSIDE 2
#define CHECKBACK 3

request req = { .checkFront = true, .checkBack = true, .checkSide = true, .checkEncoders = true,
                .calculateBack = true, .calculateSide = true, .calculateFront = true,
                .setWallAuto = true
};

int main () {
  printf(KRED "\n\n\n\n PLEASE COMPILE DOING make wall \n\n\n\n"KNRM);
  initSocket();
  encodersReset();
  sensors initial;
  double toFront;
  dist toBack = {0,0}, toSide = {0,0}, scale = {1,1};
  volts speed = {30,30};

  // This is a new cycle, first check and then move
  while(1) { cTrail(); memcpy(&initial, &r.s, sizeof(sensors));

    if (req.setWallAuto) {
      if (r.s.wall == LEFT) req.considerSide = LEFT;
      else if (r.s.wall == RIGHT) req.considerSide = RIGHT;
      else req.considerSide = 0;
      req.setWallAuto = false;
    }

    // Check sensors
    if (req.checkFront == true) { toFront = checkFront(ACCURATE); printf("toFront %lF ", toFront); req.checkFront = false; }
    if (req.checkBack == true) { toBack = checkBack(ACCURATE); printf("toBack %lF %lF ", toBack.l, toBack.r); req.checkBack = false; }
    if (req.checkSide == true) { toSide = checkSide(ACCURATE, 0, 0); printf("toSide %lF %lF ", toSide.l, toSide.r); req.checkSide == false; }
    if (req.checkEncoders == true) { encodersGet(&r.s);  printf("encoders %i %i\n", r.s.encodersL, r.s.encodersR);}
    // if (req.saveLogs == true) addLog(&r.s, &r.l);

    
    // Calculate speed
    // might be used for scale in tunnels to moderate velocity
    
    if (req.calculateBack == true) {
      
      // In case of no risk
      /**/printf("frontLRisk %d frontLRisk %d sideLRisk %d sideRRisk %d backLRisk %d backRRisk %d wall %d\n", frontLRisk, frontLRisk, sideLRisk, sideRRisk, backLRisk, backRRisk, r.s.wall);/**/
      if (!frontLRisk && !frontLRisk && !sideLRisk && !sideRRisk && !backLRisk && !backRRisk) {

        // No risks. No wall. Go straight
        if (r.s.wall == 0) { backVal = (dist) {0,0}; /**/printf("No risks. No wall. Go straight\n"); }/**/
        
        // No risks. Left wall. Touching back. We are in a deep corner. Turn
        else if (r.s.wall == LEFT && sideLInfinite && !backLisInfinite) { /**/printf("No risks. Left wall. Touching back. Turn\n");/**/

          backConsidered = toBack.l;
          backDif = 15 - backConsidered; /*/printf("backDif %i\n", backDif);/**/
          backVal = (dist) {backDif/1.5,backDif/1.5};
          // Never happened, this means it touches if very far
          if (backDif < -10) backVal = (dist){-15, -15};
          
         // No risks. Left wall. Touching back. We are in a deep corner. Turn
        } else if (r.s.wall == RIGHT && sideLInfinite && !backLisInfinite) {/**/printf("No Risks. Right Wall is being followed %i.\n", r.s.wall);/**/ 

          backConsidered = toBack.r;
          backDif = 15 - backConsidered; /*/printf("backDif %i\n", backDif);/**/
          backVal = (dist) {-backDif/1.5,-backDif/1.5};
          // Never happened, this means it touches if very far
          if (backDif < -10) backVal = (dist){+15, +15};

        }
        // You are
        else {backVal = (dist){999, 999};}
        
      // There are risks, skip
      } else { /**/printf("Risks, skip.\n");/**/ backVal = (dist){999, 999}; }
      
      printf("backVal %lF %lF\n", backVal.l, backVal.r);// TODO check the other side tooooooo!
      req.calculateBack = false;
    }
    
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateSide == true) {

      if (r.s.wall == LEFT) {
        sideConsidered = toSide.l;
        sideDif = 40 - sideConsidered; /*/printf("sideDif %i\n", sideDif);/**/
        sideVal = (dist) {sideDif, sideDif};
        if (-5 <= sideDif && sideDif <= 5) { sideVal = (dist){0,0}; /*/ printf("side in Range\n"); /**/ }
        if (sideDif < -20) sideVal = (dist){-15,-15};
      } else {
        sideConsidered = toSide.r;
        sideDif = 40 - sideConsidered; /*/printf("sideDif %i\n", sideDif);/**/
        sideVal = (dist) {-sideDif, -sideDif};
        if (-5 <= sideDif && sideDif <= 5) { sideVal = (dist){0,0}; /*/ printf("side in Range\n"); /**/ }
        if (sideDif < -20) sideVal = (dist){+15,+15};
      }
      /**/printf("sideVal %lF %lF\n", sideVal.l, sideVal.r);/**/ // TODO check the other side tooooooo!
      req.calculateSide = false;
    }
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateFront == true) {

      frontConsidered = toFront;
      frontDif = frontOk - frontConsidered; /*/printf("frontDif %i\n", frontDif);/**/
      frontVal = frontDif;
      if (-frontRange <= frontDif && frontDif <= frontRange) { frontVal = 0; /*/printf("front in Range\n");/**/ }
      if (frontVal <= -frontMinimum) { }
    
      /*/printf("frontVal %i\n", frontVal);/**/ // TODO check the other side tooooooo!
      req.calculateFront = false;
    }
    
    // Calculate scale
    
    // Check for risk
      // - recalculate speed

    if (frontVal != 999) speed = (volts){frontVal,-frontVal};
    if (sideVal.l != 999 && sideVal.r != 999) speed = (volts){sideVal.l,-sideVal.r};
    if (backVal.l != 999 && backVal.r != 999) speed = (volts){backVal.l,-backVal.r};

    // Assign voltage
    r.v = setVoltage(speed, scale);

    // Finally arrived here
    moveAtVoltage(r.v.l, r.v.r);
    printf("*  Speed chosen: %i %i\n\n", r.v.l, r.v.r );
    req = DEFAULT_REQUEST;
  }

  return 0;
}
