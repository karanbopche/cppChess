#include "pawn.h"
#include "elephant.h"
#include "king.h"
#include "queen.h"
#include "horse.h"
#include "camel.h"
#include "empty.h"

using namespace std;
// board constant definitions...............
#define P1_COLOR 	FC_CYAN
#define P2_COLOR 	FC_MAGENTA
#define FC_UNSELECTED 	FC_GREEN
#define FC_SELECTED	FC_YELLOW
#define FC_EMPTY	FC_BLUE
#define FC_KING_CHECK	FC_RED
#define MAX_MOVES	50

// box size 8x8 each box 5 char x 5 char
const int boxRowSize = 4;
const int boxColSize = 9;
const int boardWidth = 8;
const int boardHeight = 8;
// global variables...............
peice*** chessBoard;
bool running;
bool win = false;
// keep track of from where king got checked....
Point gotCheckFrom;
/// stack to undo moves....
struct Move {
Point from, to;	// peice location...
char pFrom, pTo;// peice name........

};
// stack frame.....
Move moves[MAX_MOVES];
int top=0;
// display messages.......
char msg[100];
// rederer for display.....extra space for killed objects and border....
render renderer(boardWidth*boxColSize + boxColSize*4 + 4, boardHeight*boxRowSize + 2);
// keep all killed peices..........
peice ***player1Killed;
peice ***player2Killed;
int player1KilledSlot;
int player2KilledSlot;

// flags for castling......
bool castlingP1 = true;
bool castlingP2 = true;
enum State { idle, selected };

Point player1, player2, oldLocation;	// two players and old location to move peice..........
Point *player;			// current Player pointer....

State state = idle;	// game state........

// function prototypes.........
bool init();
void resetColors();
void onEvent(keyboard Event);
void display();
void onDestroy();
bool movePeice();
bool undoMove();
int isPathBlocked(Point from, Point to);
Point getBlockingLocation(Point from, Point to);
int sign(int x);
bool checkMate(int playerId);
int isCheck(int playerId, int numChecks);
bool isCheckedInPath(Point kingLocation, Point to);
Point findKing(Point from);
void promotionLoop();
bool tryCastling();
bool canSomeoneMoveTo(Point to, int playerId);
void killedMsg(const char name, int player);

int main(){
	keyboard  key = KEY_NULL;
	if(!init()) {
		cout<<"failed to initalize"<<endl;
		return -1;
	}
	
	running = true;
	strcpy(msg,"welcome to Chess Game!");
	while(running) {
		while(renderer.getEvents(&key))
			onEvent(key);
		display();
	}
	if(win == true)
		sleep(3);
	onDestroy();	// clear all resources.....
	
}

// try to move the peice to new location.........
bool movePeice() {

	// special case for castling....
	if((castlingP1 == true || castlingP2== true) && chessBoard[oldLocation.y][oldLocation.x]->getPeiceId() == 'k' && 
		chessBoard[player->y][player->x]->getPeiceId() == 'e')
		// try to do castling if possible the do it and return........
		if(tryCastling() == true)	return true;
		

	//if peice can not move to that locaton return.... also valid for empty objects.....
	if(chessBoard[oldLocation.y][oldLocation.x]->canMove(*player) == false) {
		strcpy(msg, " piece can not move. ");
		return false;
	}
	// code to check if path is cleared.... no other object is in path.....
	if(chessBoard[oldLocation.y][oldLocation.x]->getPeiceId() != 'h')	// skip for horse...
		if(isPathBlocked(oldLocation, *player) != 0)
			return false;

	// if both location contains peice of same player......
	if(chessBoard[player->y][player->x]->getPlayerId() == chessBoard[oldLocation.y][oldLocation.x]->getPlayerId())
		return false;
		
	
	if(chessBoard[oldLocation.y][oldLocation.x]->getPeiceId() == 'p') {
		// promotion rule...........
		if(chessBoard[player->y][player->x]->getPlayerId() == 0) {
			int id = chessBoard[oldLocation.y][oldLocation.x]->getPlayerId();
			// if peice is going board upper or lower edge......
			if((id == 1 && player->y==0) || (id==2 && player->y==7)) {
				// for promotion small menu loop......
				promotionLoop();
				return true;	// undo stack is handled internally........
			}
		}
		// pawn attack case......... if moving diagonally...(attack condition)
		if(abs(oldLocation.x - player->x)) {	
			// attack digonally only if enemy peice is there.. if empty block then return...
			if(chessBoard[player->y][player->x]->getPlayerId() == 0) 
				return false;
		}
		// if moving stright then can not attack.... if enemy is in front then don't move.......
		else if(chessBoard[player->y][player->x]->getPlayerId() != 0) 
				return false;
	}
	
	// disable castling once king moved.........
	if(chessBoard[oldLocation.y][oldLocation.x]->getPeiceId() == 'k') {
		if(player == &player1)
			castlingP1 = false;
		else
			castlingP2 = false;
	}
	
	// store the move in circular buffer.....
	int t = top%MAX_MOVES;	// circle back to start.....
	moves[t].from = oldLocation;
	moves[t].to = *player;
	moves[t].pFrom = chessBoard[moves[top].from.y][moves[top].from.x]->getPeiceId();
	moves[t].pTo = chessBoard[moves[top].to.y][moves[top].to.x]->getPeiceId();
	top++;	// move stack top above....

	// swap peices if belong to seperate players or other object is empty .....
	chessBoard[oldLocation.y][oldLocation.x]->setLocation(*player);
	chessBoard[player->y][player->x]->setLocation(oldLocation);
	swap(chessBoard[oldLocation.y][oldLocation.x], chessBoard[player->y][player->x]);
	
	
	// kill other player......... if both peice belong to different player and none is empty peice.
	int id = chessBoard[oldLocation.y][oldLocation.x]->getPlayerId();
	if( id != 0) {
		// if king was killed..........	
		if(chessBoard[oldLocation.y][oldLocation.x]->getPeiceId() == 'k') {
			running = false;
			win = true;
		}
		// put the killed peice into killed slot....
		if( id == 1) {	// if killed player 1 peice.....
			// increment slot pointer..........
			player1KilledSlot++;
			// set proper location......
			chessBoard[oldLocation.y][oldLocation.x]->setLocation(Point{player1KilledSlot/boardHeight + 8, player1KilledSlot%boardHeight});
			chessBoard[oldLocation.y][oldLocation.x]->move(3, 0);	// to avoid overlap on board edge...
			player1Killed[player1KilledSlot%boardHeight][player1KilledSlot/boardHeight] = chessBoard[oldLocation.y][oldLocation.x];
		}
		else {
			// increment slot pointer.........
			player2KilledSlot++;
			chessBoard[oldLocation.y][oldLocation.x]->setLocation(Point{player2KilledSlot/boardHeight + 10, player2KilledSlot%boardHeight});
			chessBoard[oldLocation.y][oldLocation.x]->move(2, 0);	// to avoid overlap on board edge...
			player2Killed[player2KilledSlot%boardHeight][player2KilledSlot/boardHeight] = chessBoard[oldLocation.y][oldLocation.x];

		}
		killedMsg(chessBoard[oldLocation.y][oldLocation.x]->getPeiceId(), chessBoard[oldLocation.y][oldLocation.x]->getPlayerId());	// display msg of killed element....
		// allocate new empty object.........
		chessBoard[oldLocation.y][oldLocation.x] = new empty(oldLocation.x, oldLocation.y);
		chessBoard[oldLocation.y][oldLocation.x]->setColor(FC_EMPTY);
	}
	
	return true;
}

// promote the pawn to new peice.........
void promotionLoop() {
	keyboard key;
	int playerId = (player==&player1)?1:2;
	// dummy peices........
	int index = 0;
	peice *dummy[5];
	dummy[0] = new pawn(player->x, player->y, playerId);
	dummy[1] = new elephant(player->x, player->y, playerId);
	dummy[2] = new queen(player->x, player->y, playerId);
	dummy[3] = new camel(player->x, player->y, playerId);
	dummy[4] = new horse(player->x, player->y, playerId);
	
	strcpy(msg,"pawn got promotion. select peice using arrow key and press enter.");
	// store half move on stack........
	int t = top%MAX_MOVES;	// circle back to start.....
	moves[t].from = oldLocation;
	moves[t].pFrom = chessBoard[moves[top].from.y][moves[top].from.x]->getPeiceId();
	moves[t].to = *player;
	
	// swap peices to show on display .....
	chessBoard[oldLocation.y][oldLocation.x]->setLocation(*player);
	chessBoard[player->y][player->x]->setLocation(oldLocation);
	swap(chessBoard[oldLocation.y][oldLocation.x], chessBoard[player->y][player->x]);
	
	while(1) {
		if(renderer.getEvents(&key)) {
			if(key == KEY_LEFT) {
				index--;
				if(index < 0)	index = 4;
			}
			if(key == KEY_RIGHT) {
				index++;
				if(index > 4)	index = 0;
			}
			if(key == KEY_ENTER) // come out of loop.....
				break;
		}
		chessBoard[player->y][player->x] = dummy[index];
		chessBoard[player->y][player->x]->setColor(FC_SELECTED);
		display();
	}
	// set color of player.....
	chessBoard[player->y][player->x]->setColor((player==&player1)?P1_COLOR:P2_COLOR);
	// update the id of peice on undo stack......
	moves[t].pTo = chessBoard[moves[top].to.y][moves[top].to.x]->getPeiceId();
	top++;	// move stack top above....
	// free all remaining peices.......
	for(int i=0;i<5;i++) {
		if(chessBoard[player->y][player->x] != dummy[i])	// keep the selected peice......
			delete dummy[i];
	}
}

bool tryCastling() {
	int y =-1;
	if(player->y == 0 || player->y == 7) {
		if(player == &player1 && castlingP1 == true)	
			y = 7;
		else if(player == &player2 && castlingP2 == true)
			y = 0;
	}
	if(player->y==y && isPathBlocked(oldLocation, *player)==0) {
		int ex=0, kx=0;
		// if x elephant is at edge of board 
		if(player->x == 0 || player->x == 7) {
			// get elephant and king new location.......
			if(player->x == 0) 
				ex=3, kx=2;
			else if(player->x == 7) 
				ex=5, kx=6;
		
			// swap elephant left side......
			chessBoard[y][ex]->setLocation(*player);
			chessBoard[player->y][player->x]->setLocation(Point{ex, y});
			swap(chessBoard[player->y][player->x], chessBoard[y][ex]);
			// swap king lift side.......
			chessBoard[oldLocation.y][oldLocation.x]->setLocation(Point{kx, y});
			chessBoard[y][kx]->setLocation(oldLocation);
			swap(chessBoard[oldLocation.y][oldLocation.x], chessBoard[y][kx]);
		
			if(player==&player1)
				castlingP1 = false;	// no more castling allowed....
			else
				castlingP2 = false;
			return true;
		}
	}
return false;
}

// helper function to find location of king........
Point findKing(Point from) {
	int h = from.y, w;
	for(;h<boardHeight;h++) {
		w = 0;
		if(h==from.y)	w = from.x;	// only in first iteration.......
		for(;w<boardWidth;w++) {
			if(chessBoard[h][w]->getPeiceId() == 'k')
				return Point{w, h};	// return king location...
		}
	}
	return Point{-1, -1};	// king not found......
}

// see if that player's king is in check......
int isCheck(int playerId) {
	Point kingLocation = {0, 0};
	// for both kings...
	bool retValue=0;
	for(int i=0;i<2;i++) {
		kingLocation = findKing(Point{kingLocation.x+1, kingLocation.y});
		if(kingLocation.x == -1)
		// break here for debugging....
			return false;
			// move further for that perticular king only.....
		if(playerId == chessBoard[kingLocation.y][kingLocation.x]->getPlayerId())
			break;
	}
	
	//check vertically above king if king is not on top row.......
	if(kingLocation.y > 0)
	if(isCheckedInPath(kingLocation, Point{kingLocation.x, -1})==true)	retValue++;
	//check vertically below king if king is not on bottom row.......
	if(kingLocation.y < boardHeight-1)
	if(isCheckedInPath(kingLocation, Point{kingLocation.x, boardHeight})==true)	retValue++;
	//check left to king if king is not on leftmost coloumn.......
	if(kingLocation.x > 0)
	if(isCheckedInPath(kingLocation, Point{-1, kingLocation.y})==true)	retValue++;
	//check right to king.......
	if(kingLocation.x < boardWidth-1)
	if(isCheckedInPath(kingLocation, Point{boardWidth, kingLocation.y})==true)	retValue++;

	// for digonals.........
	
	int row = kingLocation.y;
	int col = kingLocation.x;
	// go to right bottom......
	if((row < boardHeight-1) && (col < boardWidth-1)) {
		for(; row<boardHeight && col<boardWidth; row++,col++);
		if(isCheckedInPath(kingLocation, Point{col, row}))	retValue++;
	}
	// go to top left......
	row = kingLocation.y;
	col = kingLocation.x;
	if(row > 0 && col > 0) {
		for(; row>=0 && col>=0; row--,col--);
		if(isCheckedInPath(kingLocation, Point{col, row}))	retValue++;
	}
	
	// go to top right......
	row = kingLocation.y;
	col = kingLocation.x;
	if((col < boardWidth-1) && (row > 0)) {
		for(; row>=0 && col<boardWidth; row--,col++);
		if(isCheckedInPath(kingLocation, Point{col, row})) 	retValue++;
	}
	// go to bottom left ......
	row = kingLocation.y;
	col = kingLocation.x;
	if(col > 0 && row < boardWidth) {
		for(; row>=0 && col>=0; row++,col--);
		if(isCheckedInPath(kingLocation, Point{col, row}))	retValue++;
	}
	// check from horse.........
	// check top sides.......
	if(kingLocation.y > 0) {
		if(kingLocation.x>0) {	// check left sides.....
			if(kingLocation.y > 1)	// if it is horse and belongs to other player.....
				if(chessBoard[kingLocation.y-2][kingLocation.x-1]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y-2][kingLocation.x-1]->getPlayerId() != playerId)
						retValue++;
			if(kingLocation.x > 1) 
				if(chessBoard[kingLocation.y-1][kingLocation.x-2]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y-1][kingLocation.x-2]->getPlayerId() != playerId)
						retValue++;
		}
		// right side.........
		if(kingLocation.x < boardWidth-1) {	// check left sides.....
			if(kingLocation.x > boardWidth-2)
				if(chessBoard[kingLocation.y-1][kingLocation.x+2]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y-1][kingLocation.x+2]->getPlayerId() != playerId)
						retValue++;
			if(kingLocation.y > 1)	 
				if(chessBoard[kingLocation.y-2][kingLocation.x+1]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y-2][kingLocation.x+1]->getPlayerId() != playerId)
						retValue++;
		}
	}
	// check bottom side.........
	if(kingLocation.y < boardHeight-1) {
		// left..........
		if(kingLocation.x>0) {
			if(kingLocation.y < boardHeight-2)
				if(chessBoard[kingLocation.y+2][kingLocation.x-1]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y+2][kingLocation.x-1]->getPlayerId() != playerId)
						retValue++;
			if(kingLocation.x > 1)
				if(chessBoard[kingLocation.y+1][kingLocation.x-2]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y+1][kingLocation.x-2]->getPlayerId() != playerId)
						retValue++;
		}
		// right........
		if(kingLocation.x < boardWidth-1) {
			if(kingLocation.y < boardHeight-2)
				if(chessBoard[kingLocation.y+2][kingLocation.x+1]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y+2][kingLocation.x+1]->getPlayerId() != playerId)
						retValue++;
			if(kingLocation.x < boardWidth-2)
				if(chessBoard[kingLocation.y+1][kingLocation.x+2]->getPeiceId() == 'h' && 
				chessBoard[kingLocation.y+1][kingLocation.x+2]->getPlayerId() != playerId)
						retValue++;
		}
	}
	return retValue;
}

// see if checkmate happened...........
bool checkMate(int playerId, int numChecks) {
	Point p = {0, 0};
	for(int i=0;i<2;i++) {
		p =  findKing(Point{p.x+1, p.y});
		if(p.x == -1)
			return false;	// debug break point here.........
		if(chessBoard[p.y][p.x]->getPlayerId() == playerId)	break;
	}
	// set oldLocation to kings Location to move it...
	oldLocation = p;
	// set player to start of loop...
	player->y = (p.y-1 < 0)?0:p.y-1;
	Point to;
	// set end of the loop.......
	to.x = (p.x+1 < boardWidth)?p.x+1:p.x;
	to.y = (p.y+1 < boardHeight)?p.y+1:p.y;
	// move king to all possible location and see if he is still getting check...
	for(;player->y<=to.y; player->y++) {
	// rest start of loop in each iteration.......
		player->x = (p.x-1 < 0)?0:p.x-1;
		for(;player->x<=to.x; player->x++)
			if(movePeice()== true) {	// if king can move to that location 
			//  is he geting check ..........
			// switch player to keep consistancy.....
				if(playerId == 1)	player = &player2;
				else			player = &player1;
				if(isCheck(playerId) == false) { // if king is not getting check there
					undoMove();
					strcpy(msg, "king in check!");
					// to be consistant with player switch in event function....
					if(player == &player1)	player = &player2;
					else			player = &player1;
					return false;
				}
				undoMove();
			}
	}
	// if more than one check is given to king and king can't move to safe place....
	if(numChecks > 1) {
		if(playerId == 1)
			strcpy(msg, "player 1 got checkMate");
		else
			strcpy(msg, "player 2 got checkMate");
			return true;
	}
	// if check can be avoided by moving other peices...........
	// path for loop iterators.......
	int dx = sign(gotCheckFrom.x - p.x);
	int dy = sign(gotCheckFrom.y - p.y);
	p.x += dx;
	p.y += dy;
	// loop in path......
	for(;(p.y!=gotCheckFrom.y || p.x != gotCheckFrom.x) && p.x < boardWidth && p.y < boardHeight ;p.y+=dy, p.x+=dx)
	{
		if(canSomeoneMoveTo(p, playerId) == true) {// if any peice can come in path to block check it's not a checkmate ....
			strcpy(msg, "king in check!");	// default msg..		
			return false;
		}
	}
	if(canSomeoneMoveTo(p, playerId) == true) {// if any peice can kill that peice to cancel check it's not a checkmate .....
		strcpy(msg, "king in check!");	// default msg..			
		return false;
	}
	// king got check in all places...hence check mate....
	if(playerId == 1)
	strcpy(msg, "player 1 got checkMate");
	else
	strcpy(msg, "player 2 got checkMate");
	return true;
} 

bool canSomeoneMoveTo(Point to, int playerId) {
	Point p;
	*player = to;
	// if vertically above player's peice is present check if he can come to TO location...
	if(isPathBlocked(to, Point{to.x, -1}) == playerId) {
		p = getBlockingLocation(to, Point{to.x, -1});
		oldLocation = p;	// to check if peice can move....
		if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
			if(playerId == 1) player = &player2;
			else		  player = &player1;
			undoMove();
			if(player == &player1)	player = &player2;
			else			player = &player1;
			return true;
		}
	}
	// if vertically below player's peice is present check if he can come to TO location...
	if(isPathBlocked(to, Point{to.x, 8}) == playerId) {
		p = getBlockingLocation(to, Point{to.x, 8});
		oldLocation = p;	// to check if peice can move....
		if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
			if(playerId == 1) player = &player2;
			else		  player = &player1;
			undoMove();
			if(player == &player1)	player = &player2;
			else			player = &player1;
			return true;
		}
	}
	// if in left player's peice is present check if he can come to TO location...
	if(isPathBlocked(to, Point{-1, to.y}) == playerId) {
		p = getBlockingLocation(to, Point{-1, to.y});
		oldLocation = p;	// to check if peice can move....
		if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
			if(playerId == 1) player = &player2;
			else		  player = &player1;
			undoMove();
			if(player == &player1)	player = &player2;
			else			player = &player1;
			return true;
		}
	}
	// if in right player's peice is present check if he can come to TO location...
	if(isPathBlocked(to, Point{8, to.y}) == playerId) {
		p = getBlockingLocation(to, Point{8, to.y});
		oldLocation = p;	// to check if peice can move....
		if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
			if(playerId == 1) player = &player2;
			else		  player = &player1;
			undoMove();
			if(player == &player1)	player = &player2;
			else			player = &player1;
			return true;
		}
	}
	
	// digonals...........
	int row = to.y;
	int col = to.x;
	// go to right bottom......
	if((row < boardHeight-1) && (col < boardWidth-1)) {
		// go to digonal end....
		for(; row<boardHeight && col<boardWidth; row++,col++);
		
		if(isPathBlocked(to, Point{col, row}) == playerId) {
			p = getBlockingLocation(to, Point{col, row});
			oldLocation = p;	// to check if peice can move....
			if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
				if(playerId == 1) player = &player2;
				else		  player = &player1;
				undoMove();
				if(player == &player1)	player = &player2;
				else			player = &player1;
				return true;
			}
		}
	}
	// go to top left......
	row = to.y;
	col = to.x;
	if(row > 0 && col > 0) {
		for(; row>=0 && col>=0; row--,col--);
		if(isPathBlocked(to, Point{col, row}) == playerId) {
			p = getBlockingLocation(to, Point{col, row});
			oldLocation = p;
			if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
				if(playerId == 1) player = &player2;
				else		  player = &player1;
				undoMove();
				if(player == &player1)	player = &player2;
				else			player = &player1;
				return true;
			}
		}
	}
	// go to top right......
	row = to.y;
	col = to.x;
	if((col < boardWidth-1) && (row > 0)) {
		for(; row>=0 && col<boardWidth; row--,col++);
		if(isPathBlocked(to, Point{col, row}) == playerId) {
			p = getBlockingLocation(to, Point{col, row});
			oldLocation = p;
			if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
				if(playerId == 1) player = &player2;
				else		  player = &player1;
				undoMove();
				if(player == &player1)	player = &player2;
				else			player = &player1;
				return true;
			}
		}
	}
	// go to bottom left ......
	row = to.y;
	col = to.x;
	if(col > 0 && row < boardWidth) {
		for(; row>=0 && col>=0; row++,col--);
		if(isPathBlocked(to, Point{col, row}) == playerId) {
			p = getBlockingLocation(to, Point{col, row});
			oldLocation = p;
			if(chessBoard[p.y][p.x]->getPeiceId() != 'k' && movePeice() == true) {
				if(playerId == 1) player = &player2;
				else		  player = &player1;
				undoMove();
				if(player == &player1)	player = &player2;
				else			player = &player1;
				return true;
			}
		}
	}
	
	// check if horse can move to that location.........
	// check top sides.......
	if(to.y > 0) {
		if(to.x>0) {	// check left sides.....
			if(to.y > 1)	// if it is horse and belongs to same player.....
				if(chessBoard[to.y-2][to.x-1]->getPeiceId() == 'h' && 
				chessBoard[to.y-2][to.x-1]->getPlayerId() == playerId)
						return true;
			if(to.x > 1) 
				if(chessBoard[to.y-1][to.x-2]->getPeiceId() == 'h' && 
				chessBoard[to.y-1][to.x-2]->getPlayerId() != playerId)
						return true;
		}
		// right side.........
		if(to.x < boardWidth-1) {	// check left sides.....
			if(to.x > boardWidth-2)
				if(chessBoard[to.y-1][to.x+2]->getPeiceId() == 'h' && 
				chessBoard[to.y-1][to.x+2]->getPlayerId() != playerId)
						return true;
			if(to.y > 1)	 
				if(chessBoard[to.y-2][to.x+1]->getPeiceId() == 'h' && 
				chessBoard[to.y-2][to.x+1]->getPlayerId() != playerId)
						return true;
		}
	}
	// check bottom side.........
	if(to.y < boardHeight-1) {
		// left..........
		if(to.x>0) {
			if(to.y < boardHeight-2)
				if(chessBoard[to.y+2][to.x-1]->getPeiceId() == 'h' && 
				chessBoard[to.y+2][to.x-1]->getPlayerId() != playerId)
						return true;
			if(to.x > 1)
				if(chessBoard[to.y+1][to.x-2]->getPeiceId() == 'h' && 
				chessBoard[to.y+1][to.x-2]->getPlayerId() != playerId)
						return true;
		}
		// right........
		if(to.x < boardWidth-1) {
			if(to.y < boardHeight-2)
				if(chessBoard[to.y+2][to.x+1]->getPeiceId() == 'h' && 
				chessBoard[to.y+2][to.x+1]->getPlayerId() != playerId)
						return true;
			if(to.x < boardWidth-2)
				if(chessBoard[to.y+1][to.x+2]->getPeiceId() == 'h' && 
				chessBoard[to.y+1][to.x+2]->getPlayerId() != playerId)
						return true;
		}
	}
	return false;
}

// see if king got check in this path.........
bool isCheckedInPath(Point kingLocation, Point to) {

	int flag = isPathBlocked(kingLocation, to);
	// path blocked by enemy player peice..........
	if((flag != 0) && (flag != chessBoard[kingLocation.y][kingLocation.x]->getPlayerId())) {
		// get location of enemy....
		Point enemy = getBlockingLocation(kingLocation, to);
		// no one is blocking the the way......
		if(enemy.x == -1)	
			return false;
		//if that player can attack king....
		if(chessBoard[enemy.y][enemy.x]->canMove(kingLocation)) {
		// indicate king is in check.........
			chessBoard[kingLocation.y][kingLocation.x]->setColor(FC_KING_CHECK);
			chessBoard[enemy.y][enemy.x]->setColor(FC_KING_CHECK);
			strcpy(msg,"king is in check !");
			gotCheckFrom = enemy;	// keep track of location of enemy.........
			return true;
		}
	}
	return false;
}

// check if path is clear for peice to move if blocked then return which player's peice is blocking the path............
int isPathBlocked(Point from, Point to) {
	// path loop iterators.......
	int dx = sign(to.x - from.x);
	int dy = sign(to.y - from.y);
	from.x += dx;
	from.y += dy;
	// loop in path......
	for(;(from.y!=to.y || from.x != to.x) && from.x<boardWidth && from.y<boardHeight ;from.y+=dy, from.x+=dx)
	{
		if(chessBoard[from.y][from.x]->getPlayerId() != 0)	 // if any peice found in path return false...
			return chessBoard[from.y][from.x]->getPlayerId();
	}
	return 0;
}

// return the location of point at which path of peice is blocked......
Point getBlockingLocation(Point from, Point to) {
	// path loop iterators.......
	int dx = sign(to.x - from.x);
	int dy = sign(to.y - from.y);
	from.x += dx;
	from.y += dy;
	// loop in path......
	for(;(from.y!=to.y || from.x != to.x) && from.x<boardWidth && from.y<boardHeight ;from.y+=dy, from.x+=dx)
	{
		if(chessBoard[from.y][from.x]->getPlayerId() != 0)	 // if any peice found in path return false...
			return from;
	}
	return Point{-1, -1};
}

// handle the events.......
void onEvent(keyboard event){
	static const char* color = FC_UNSELECTED;
	if(event == KEY_Q)
		running = false;
	
	// reset color of older peice.........
	chessBoard[player->y][player->x]->setColor(FC_EMPTY);	// reset color.....
	if(chessBoard[player->y][player->x]->getPlayerId() == 1)	// set color acording to player.....
		chessBoard[player->y][player->x]->setColor(P1_COLOR);
	else if(chessBoard[player->y][player->x]->getPlayerId() == 2)
		chessBoard[player->y][player->x]->setColor(P2_COLOR);
	
	if(event == KEY_LEFT) 
		player->x = (player->x -1 + boardWidth)%boardWidth;	
	if(event == KEY_RIGHT)
		player->x = (player->x + 1 )%boardWidth;
	if(event == KEY_UP)
		player->y = (player->y - 1 + boardWidth)%boardWidth;
	if(event == KEY_DOWN)
		player->y = (player->y + 1 )%boardWidth;
	if(event == KEY_U)
		undoMove();
	// rest message........
	msg[0] = 0;
	if(event == KEY_ENTER) {
		if(state == idle) {		// state machine..........
			// check if selected peice belong to that player.....
			if(player == &player1) {	// for player1
				if(chessBoard[player->y][player->x]->getPlayerId() == 1) {
					oldLocation = *player;	// save current location of selected peice.
					state = selected;
					color = FC_SELECTED;
				}
			}
			else {	// same for player 2............
				if(chessBoard[player->y][player->x]->getPlayerId() == 2) {
					oldLocation = *player;	// save current location of selected peice.
					state = selected;
					color = FC_SELECTED;
				}
			}
		}
		else if(state == selected) {
			if(movePeice()) {
	// check if kings got check....
				resetColors();	// reset colors to default....
				int cFlagKing1 = isCheck(1);	// temporary store check status...
				int cFlagKing2 = isCheck(2);
	// if current move makes our king to get check.....invalid move...
				if((player==&player1 && cFlagKing1>0)||(player==&player2 && cFlagKing2>0)) {
					// turn off castling for that player........
					if(player == &player1)
						castlingP1 = false;
					else
						castlingP2 = false;
					undoMove();
					strcpy(msg, " Invalid Move. ");	
				}
	// check happend to other player...do checkmate logic.. logic.......
				else if(cFlagKing1) {
					if(checkMate(1, cFlagKing1) == true) {
						running = false;
						win = true;
						strcat(msg, "player 2 won !!");
						return;
					}
				}
				else if (cFlagKing2) {
					if(checkMate(2, cFlagKing2) == true) {
						running = false;
						win = true;
						strcat(msg, " player 1 won !!");
						return;
					}
				}
	// player switch logic......
				if(player == &player1)	player = &player2;
				else			player = &player1;
	// set player location to default locations......
				player1.x = 3;
				player1.y = 7;
				player2.x = 3;
				player2.y = 0;	
			}
			state = idle;
			color = FC_UNSELECTED;
			
		}
	}
	// set color for current peice.........
	chessBoard[player->y][player->x]->setColor(color);
	if(player == &player1)
		strcat(msg, " Player 1 turn.");
	else
		strcat(msg, " Player 2 turn.");
}
// initialize the whole chess board........
bool init()
{
	// create empty board .........
	chessBoard = new peice**[boardHeight];
	for(int h=0;h<boardHeight;h++)		
		chessBoard[h] = new peice*[boardWidth];
	// initialize board..........
	for(int i=0;i<boardWidth;i++) {
		// row 6 and 2 initialize with pawn.......
		chessBoard[6][i] = new pawn(i, 6, 1);
		chessBoard[1][i] = new pawn(i, 1, 2);
	}
	// allocatte each peice to chess board and set color for each player.......
	chessBoard[0][0] = new elephant(0, 0, 2);
	chessBoard[7][0] = new elephant(0, 7, 1);
	chessBoard[7][1] = new horse(1, 7, 1);
	chessBoard[0][1] = new horse(1, 0, 2);
	chessBoard[7][2] = new camel(2, 7, 1);
	chessBoard[0][2] = new camel(2, 0, 2);
	chessBoard[7][3] = new queen(3, 7, 1);
	chessBoard[0][3] = new queen(3, 0, 2);
	chessBoard[7][4] = new king(4, 7, 1);
	chessBoard[0][4] = new king(4, 0, 2);
	chessBoard[7][5] = new camel(5, 7, 1);
	chessBoard[0][5] = new camel(5, 0, 2);
	chessBoard[7][6] = new horse(6, 7, 1);
	chessBoard[0][6] = new horse(6, 0, 2);
	chessBoard[7][7] = new elephant(7, 7, 1);
	chessBoard[0][7] = new elephant(7, 0, 2);
	
	// empty objects...............
	for(int row=2;row<6;row++)
		for(int col=0;col<boardWidth;col++) {
			chessBoard[row][col] = new empty(col, row);
		}
	resetColors();	// set default colors of peices....
	// array for killed peices........
	player1Killed = new peice**[boardHeight];
	player2Killed = new peice**[boardHeight];
	for(int i=0;i<boardHeight;i++) {
		player1Killed[i] = new peice*[2];
		player2Killed[i] = new peice*[2];
	}
	// killed slot positions......
	player1KilledSlot = -1;
	player2KilledSlot = -1;
	// player initial positions.........
	player1.x = 3;
	player1.y = 7;
	player2.x = 3;
	player2.y = 0;
	oldLocation.x = -1;
	oldLocation.y = -1;
	// current player pointer...........
	player = &player1;
	
	// set initial selected color for first peice.......
	chessBoard[player->y][player->x]->setColor(FC_UNSELECTED);
	return true;
}

void resetColors() {
	for(int row=0;row<boardHeight;row++)
		for(int col=0;col<boardWidth;col++){
		int id = chessBoard[row][col]->getPlayerId();
		if(id == 0)	// empty box...
			chessBoard[row][col]->setColor(FC_EMPTY);
		else if(id == 1)
			chessBoard[row][col]->setColor(P1_COLOR);
		else if(id == 2)
			chessBoard[row][col]->setColor(P2_COLOR);
	}
}

// display all the elements of game on terminal............
void display() {
	renderer.clear();
	// draw the peices........
	for(int row=0;row<boardHeight;row++)
		for(int col=0;col<boardWidth;col++){
				renderer.print(*chessBoard[row][col]);
		}
	// draw vertical divider line.......
	renderer.drawLine(Point{boardWidth*boxColSize + 1, 0}, Point{boardWidth*boxColSize + 1, boardHeight*boxRowSize}, '|');
	// horizontal line..........
	renderer.drawLine(Point{0,boardHeight*boxRowSize}, Point{(boardWidth+4)*boxColSize+3,boardHeight*boxRowSize}, '-');
	// draw killed peices..........

	for(int row=0;row<boardHeight;row++)
		for(int col=0;col<2;col++) {
			// only if there is a peice......
			if(player2Killed[row][col] != NULL)
				renderer.print(*player2Killed[row][col]);
			if(player1Killed[row][col] != NULL)
				renderer.print(*player1Killed[row][col]);
		}
	// display message on bottom......
	if(msg[0])
		renderer.drawText( 3, boardHeight*boxRowSize + 1, msg);
	// flush the buffer to display........
	renderer.update();
}
// on exit clear all the memory.........
void onDestroy() {
	// delete each peice first........
	for(int row=0;row<boardHeight;row++)
		for(int col=0;col<boardWidth;col++)
			delete chessBoard[row][col];
	// delete columns of chess board......
	for(int row=0;row<boardHeight;row++)
		delete[] chessBoard[row];
	// delete rows of chess board.....
	delete[] chessBoard;
	// delete the killed peices..........
	for(int row=0;row<boardHeight;row++) {
		delete[] player1Killed[row];
		delete[] player2Killed[row];
	}
	delete[] player1Killed;
	delete[] player2Killed;
}
// helper math function........
int sign(int x) {
	if(x>0)	return 1;	//positive number......
	else if(x<0) return -1;	// negative number........
	return 0;
}

void killedMsg(const char name, int playerId) {
	msg[0] = 0;
	switch(name) {
	case 'k': 	strcat(msg, "King"); 	break;
	case 'r': 	strcat(msg, "Rook"); 	break;
	case 'p': 	strcat(msg, "Pawn"); 	break;
	case 'c': 	strcat(msg, "Camel"); 	break;
	case 'q': 	strcat(msg, "Queen"); 	break;
	case 'h': 	strcat(msg, "Horse"); 	break;
	default :	strcat(msg, "invalid"); break;
	}
	strcat(msg, " of player ");
	if(playerId == 1)
		strcat(msg, "1");
	else if(playerId == 2)
		strcat(msg, "2");
	strcat(msg, " was killed !");
	
}

// undo last move...........
bool undoMove() {
	Move pop;
	if(top <= 0)
		return false;
	top--;
	pop = moves[top%MAX_MOVES];
	
	// toggle player so the player will contain ID of player who moved....
	if(player == &player1)	player = &player2;
	else			player = &player1;

	// swap peices and set locations..........
	chessBoard[pop.from.y][pop.from.x]->setLocation(pop.to);
	chessBoard[pop.to.y][pop.to.x]->setLocation(pop.from);
	swap(chessBoard[pop.from.y][pop.from.x], chessBoard[pop.to.y][pop.to.x]);

	// if the peice was killed at 'to' location....
	if(chessBoard[pop.to.y][pop.to.x]->getPeiceId() != pop.pTo) {
		// delete the empty peice......
		delete chessBoard[pop.to.y][pop.to.x];
		// player one turn then killed peice was of player 2 
		if(player == &player1) {
			chessBoard[pop.to.y][pop.to.x] = player2Killed[player2KilledSlot%boardHeight][player2KilledSlot/boardHeight];
			player2Killed[player2KilledSlot%boardHeight][player2KilledSlot/boardHeight] = NULL;
			player2KilledSlot--;
		}
		else{
			chessBoard[pop.to.y][pop.to.x] = player1Killed[player1KilledSlot%boardHeight][player1KilledSlot/boardHeight];
			player1Killed[player1KilledSlot%boardHeight][player1KilledSlot/boardHeight] = NULL;
			player1KilledSlot--;
		}
		chessBoard[pop.to.y][pop.to.x]->setLocation(pop.to);
	}	
	return true;
}


