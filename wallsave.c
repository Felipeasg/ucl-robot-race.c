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

request req = { .checkFront = false, .checkBack = true, .checkSide = false, .checkSideStatic = true, .checkFrontStatic = true, .checkEncoders = true,
                .calculateBack = true, .calculateSide = true, .calculateFront = false,
                .setWallAuto = true, .rotate180 = true };

int main () {
  initSocket();
  encodersReset();
  sensors initial;
  double toFront;
  dist toBack = {0,0}, toSide = {0,0}, scale = {1,1};
  volts speed = {30,30};

  // This is a new cycle, first check and then move
  while(1) { memcpy(&initial, &r.s, sizeof(sensors));

    if (req.setWallAuto) {
      if (r.s.wall == LEFT) req.considerSide = LEFT;
      else if (r.s.wall == RIGHT) req.considerSide = RIGHT;
      else req.considerSide = 0;
      req.setWallAuto = false;
    }

    if (req.checkSideStatic == true) { toSide = checkSideStatic(ACCURATE); printf("toSide %lF %lF ", toSide.l, toSide.r); req.checkSideStatic == false; }
    if (req.checkFrontStatic == true) { toFront = checkFrontStatic(ACCURATE); printf("toFront %lF ", toFront); req.checkFrontStatic = false; }
    if (req.checkFront == true) { toFront = checkFront(ACCURATE); printf("toFront %lF ", toFront); req.checkFront = false; }
    if (req.checkSide == true) { toSide = checkSide(ACCURATE, 0, 0); printf("toSide %lF %lF ", toSide.l, toSide.r); req.checkSide == false; }
    if (req.checkBack == true) { toBack = checkBack(ACCURATE); printf("toBack %lF %lF ", toBack.l, toBack.r); req.checkBack = false; }

    if (req.checkEncoders == true) { encodersGet(&r.s);  printf("encoders %i %i\n", r.s.encodersL, r.s.encodersR);}


    if (req.rotate180 == true) {
      if (frontRisk && sideLRisk && sideRRisk) {
        reposition(&r, 400, 400, 10, -10); 
        req = DEFAULT_REQUEST;
        continue;
      }
    }
    
    if (req.calculateBack == true) {
      
      // In case of no risk
      /**/printf("frontLRisk %d frontLRisk %d sideLRisk %d sideRRisk %d backLRisk %d backRRisk %d wall %d\n", frontLRisk, frontLRisk, sideLRisk, sideRRisk, backLRisk, backRRisk, r.s.wall);/**/
      if (!frontLRisk && !frontRRisk && !sideLRisk && !sideRRisk && !backLRisk && !backRRisk) {

        // Case 1: No risks. No wall. Go straight
        if (r.s.wall == 0) { backVal = (dist) {0,0}; /**/printf("No risks. No wall. Go straight\n"); }/**/
        
        // Case 2: No risks. Left wall. Touching back. We are in a deep corner. Turn
        else if (r.s.wall == LEFT && r.s.rangeFL >= 40 && !backLisInfinite) { /**/printf("No risks. Left wall. Touching back. Turn\n");/**/

          backConsidered = toBack.l;
          backDif = 15 - backConsidered;
          
          if (backConsidered < 38 && backConsidered >= 20)      {backVal = (dist) {-20,-20};}                 // Too far
//          else if (backConsidered < 20 && backConsidered >= 15) {backVal = (dist) {backDif/1.5,backDif/1.5};} // Far
          else if (backConsidered < 20 && backConsidered >= 15) {backVal = (dist) {0,0};}
          else if (backConsidered < 15 && backConsidered >= 10) {backVal = (dist) {-backDif/1.5,-backDif/1.5};} // In range

          
        // Case 3: No risks. Left wall. Touching back. We are in a deep corner. Turn
        } else if (r.s.wall == RIGHT && sideLInfinite && !backLisInfinite) {/**/printf("No Risks. Right Wall is being followed %i.\n", r.s.wall);/**/ 

          backConsidered = toBack.r;
          backDif = 15 - backConsidered;

          if (backConsidered < 38 && backConsidered >= 20)      {backVal = (dist) {+20,+20};}                 // Too far
          else if (backConsidered < 20 && backConsidered >= 15) {backVal = (dist) {-backDif/1.5,-backDif/1.5};} // Far
          else if (backConsidered < 15 && backConsidered >= 10) {backVal = (dist) {-backDif/1.5,-backDif/1.5};} // In range

        }
        // Default
        else {backVal = (dist){999, 999};}
        
      // Risks, skip
      } else { /**/printf("Risks, skip.\n");/**/ backVal = (dist){999, 999}; }
      
      /*/printf("backVal %lF %lF\n", backVal.l, backVal.r); /**/ // TODO check the other side tooooooo!
      req.calculateBack = false;
    }
    
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateSide == true) {


        if (r.s.wall == LEFT) {
          sideConsidered = toSide.l;
          sideDif = 30 - sideConsidered; /*/printf("sideDif %i\n", sideDif);/**/

          if (r.s.rangeFL < 100 && r.s.rangeFL >= 60)     { sideVal = (dist) {0,0};}          // Too far
          else if (r.s.rangeFL < 60 && r.s.rangeFL >= 55) { sideVal = (dist) {0, 0};} // Far
          else if (r.s.rangeFL < 55 && r.s.rangeFL >= 28) { sideVal = (dist) {0, 0};}             // In range
          else if (r.s.rangeFL < 15)                         { sideVal = (dist) {sideDif, sideDif}; printf("HEREHRERE\n");} // Close

        } else {
          sideConsidered = toSide.r;
          sideDif = 40 - sideConsidered; /*/printf("sideDif %i\n", sideDif);/**/

          if (sideConsidered < 100 && sideConsidered >= 60)     { sideVal = (dist) {+15,+15};}            // Too far
          else if (sideConsidered < 60 && sideConsidered >= 55) { sideVal = (dist) {-sideDif, -sideDif};} // Far
          else if (sideConsidered < 55 && sideConsidered >= 45) { sideVal = (dist) {0, 0};}               // In range
          else if (sideConsidered < 45)                         { sideVal = (dist) {-sideDif, -sideDif};} // Close
        }

      /*/printf("sideVal %lF %lF\n", sideVal.l, sideVal.r);/**/ // TODO check the other side tooooooo!
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
