#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <fstream>
#define OPEN_WBO_DIR 
using namespace std;


const int surround_x[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
const int surround_y[8] = {-1,  0,  1, -1,  1, -1,  0,  1};



class edge{
	public:
	int x1, y1, x2, y2;
	bool vaild;
	edge(int v1, int v2, int v3, int v4){
		x1 = v1;
		y1 = v2;
		x2 = v3;
		y2 = v4;
		vaild = true;
	}
	edge(bool v){
		vaild = v; 
	}
};

void slotEdgeCondition(ostream&, edge, int, int, int, int, int);

int main(){
	int x, y;
	cin >> x >> y;
	ofstream ofile("clauses.cnf", ios::out | ios::trunc);
	ofstream &ofile_ref = ofile;
	ofile << "p cnf                                       " << endl;
	int clauseNum = 0;

	vector<vector<int>> board(2*x +1 , vector<int>(2*y+1, 0));
	for(int i=1; i<2*x; i+=2){
		for(int j=1; j<2*y; j+=2){		
			cin >> board[i][j];
			if(board[i][j] == 0) board[i][j] = -1;
		}
	}
	
	for (int i=0; i<2*x+1; i++){
		for(int j=0; j<2*y+1; j++) {
			if((i==0 || i==2*x) && (j==0 || j==2*y)) board[i][j] = -1;
			
			if (board[i][j] == -1) cout << "x ";
			else cout << board[i][j] << " ";
		}
		cout << endl;
	}

	
	vector<edge> edge_on_node[2*x+1][2*y+1];
	for(int i=1; i<2*y; i++){
		edge upper_edge(0, i, 1, i);
		edge_on_node[0][i].push_back(upper_edge);
		edge lower_edge(2*x, i, 2*x-1, i);
		edge_on_node[2*x][i].push_back(lower_edge);
	}
	for(int i=1; i<2*x; i++){
		edge right_edge(i, 0, i, 1);
		edge_on_node[i][0].push_back(right_edge);
		edge left_edge(i, 2*y, i, 2*y-1);
		edge_on_node[i][2*y].push_back(left_edge);
	}
	
	int gridNum = 0;
	for(int i=2; i<2*x-1; i+=2){
		for(int j=2; j<2*y-1; j+=2){
			for (int sur=0; sur<8; sur++){
				if(board[i+surround_x[sur]][j+surround_y[sur]] == -1){
					edge invaild_edge(false);
					edge_on_node[i][j].push_back(invaild_edge);
				}else{
					edge new_edge(i, j, i+surround_x[sur], j+surround_y[sur]);
					edge_on_node[i][j].push_back(new_edge);
				}
			}
			gridNum++;
		}
	}

	cout << endl;
	/*
	for(int i=0; i<2*x+1; i++) {
		for(int j=0; j<2*y+1; j++) cout << edge_on_node[i][j].size() << " ";
		cout << endl;
	}
	*/
	//Variable:
	//GRID edge(a node make 8 varible)//
	//SLOT edge(a node make only 1 varible)//
	//boardNum (1 for 1, 0 for 2)//
	int slotNum = 4*x + 4*y - 4;
	int slotVarShift = 8 * gridNum + 1;
	int	boardVarShift = slotVarShift + slotNum; 
	//cout << "***" << boardVarShift << "***" << slotVarShift << endl;



	int correntGrid = 0;
	for(int i=2; i<2*x-1; i+=2){
		for(int j=2; j<2*y-1; j+=2){
			int vaildTest = (1<<8) - 1;
			int invaildNum = 0;
			for (int sur=0; sur<8; sur++){
			//		bitset<32> aa(vaildTest);
			//		cout << i+surround_x[sur] << " " << j+surround_y[sur] << " " << aa<< endl;
				if(board[i+surround_x[sur]][j+surround_y[sur]] == -1){
					vaildTest ^= 1<<sur;
					invaildNum++;
				}
			}
			
			for	(int v=0; v < (1<<8)-1; v++){ // -1 for not compute the all negtive, bit 1 for neg, bit 0 for pas or invaild
				if(!(v^vaildTest)&vaildTest) continue;
				
				int zeroNum = 0;
				for (int t=0; t<8; t++) if (!zeroNum>>t) zeroNum++;
				if (zeroNum == invaildNum + 2) continue;
				
				for (int t=0; t<8; t++){
					if (!(1<<t & vaildTest)) continue;
					if (!(1<<t & v)) ofile << (correntGrid * 8 + t + 1) * (-1)<< " ";
					else ofile << correntGrid * 8 + t + 1 << " ";
				}
				ofile << endl;
				clauseNum++;
			}
			correntGrid++;
		}
	}

	correntGrid = 0;
	for(int i=2; i<2*x-1; i+=2){
		for(int j=2; j<2*y-1; j+=2){
			for (int sur=0; sur<8; sur++){
				// remeber + 1
				if(edge_on_node[i][j][sur].vaild == false) continue;
				ofile << (correntGrid * 8 + sur + 1) * -1 << " " 
					<< (boardVarShift + edge_on_node[i][j][sur].x1 * (2*y+1) + edge_on_node[i][j][sur].y1)*-1 << " "
					<< boardVarShift + edge_on_node[i][j][sur].x2 * (2*y+1) + edge_on_node[i][j][sur].y2 << endl;	
				ofile << (correntGrid * 8 + sur + 1) * -1 << " " 
					<< boardVarShift + edge_on_node[i][j][sur].x1 * (2*y+1) + edge_on_node[i][j][sur].y1 << " "
					<< (boardVarShift + edge_on_node[i][j][sur].x2 * (2*y+1) + edge_on_node[i][j][sur].y2)*-1 << endl;	
				clauseNum += 2;
			}

			correntGrid++;
		}
	}
	//size: 2*x-1 upper
	//size: 2*x-1 lower
	//size: 2*y-1 right
	//size: 2*y-1 left
	int correntSlotShift = 0;
	for(int j=1; j<2*y; j++){
		if (board[edge_on_node[0][j][0].x2][edge_on_node[0][j][0].y2] != -1){
			slotEdgeCondition(ofile_ref, edge_on_node[0][j][0], x, y, j-1, slotVarShift, boardVarShift);
			clauseNum += 2;
		}

		if (board[edge_on_node[2*x][j][0].x2][edge_on_node[2*x][j][0].y2] != -1){
			slotEdgeCondition(ofile_ref, edge_on_node[2*x][j][0], x, y, (2*x-1) + j -1, slotVarShift, boardVarShift);
			clauseNum += 2;
		}
	}
	for(int i=1; i<2*x; i++){
		if (board[edge_on_node[i][0][0].x2][edge_on_node[i][0][0].y2] != -1){
			slotEdgeCondition(ofile_ref, edge_on_node[i][0][0], x, y, (2*x-1)*2 + i-1, slotVarShift, boardVarShift);
			clauseNum += 2;
		}
		if (board[edge_on_node[i][2*y][0].x2][edge_on_node[i][2*y][0].y2] != -1){
			slotEdgeCondition(ofile_ref, edge_on_node[i][2*y][0], x, y, (2*x-1)*2 + (2*y-1) + i -1, slotVarShift, boardVarShift);	
			clauseNum += 2;
		}
	}

	//Slot a < b, but we set 1 be pass and 2 be neg so it would be A or !B
	for(int i=0; i < 4*x+4*y-4-1; i++) {
		clauseNum++;
		ofile << slotVarShift + i << " " << (slotVarShift + i + 1)*-1 << endl;
	}
	//Pin only one edge connected
	for(int i=1; i<2*x; i+=2){
		for(int j=1; j<2*y; j+=2){
			if(board[i][j] == -1) continue;
			vector<int> edgeVar;
			if(i-1 == 0) edgeVar.push_back(slotVarShift + j-1);
			if(i+1 == 2*x) edgeVar.push_back(slotVarShift + (2*x-1) + j-1);
			if(j-1 == 0) edgeVar.push_back(slotVarShift + (2*x-1)*2 + i-1);
			if(j+1 == 2*y) edgeVar.push_back(slotVarShift + (2*x-1)*2 + (2*y-1) + i-1);
			
			int sur_dia[4] = {0, 2, 5, 7};
			for(int s=0; s<4; s++){
				int tmp_x =  i + surround_x[sur_dia[s]];
				int tmp_y =  j + surround_y[sur_dia[s]];

				vector<edge> &tmp = edge_on_node[tmp_x][tmp_y];
				if(tmp.size() != 8) continue;
				edgeVar.push_back(((tmp_x/2 - 1)*(y-1) + (tmp_y/2-1)) * 8 + sur_dia[3-s] + 1);
			}

			for(int v=0; v< (1<<edgeVar.size()); v++){
				int zeroNum = 0; //bit
				for(int k=0; k<edgeVar.size(); k++) if(!((v>>k)%2)) zeroNum++;
				if(zeroNum == 1) continue;
				for(int k=0; k<edgeVar.size(); k++){
					if(!((v>>k)%2)) ofile << edgeVar[k]*(-1) <<" ";
					else ofile << edgeVar[k] <<" ";
				}
				ofile << " "<<v<< endl;
				clauseNum++;
			}
		}
	}

	int c = 0;
	for (int i=0; i<2*x; i++){
		for(int j=0; j<2*y; j++) {
			if(board[i][j] == 1){ofile << boardVarShift + i*(2*y+1) + j << endl; clauseNum++;}
			if(board[i][j] == 2){ofile << (boardVarShift + i*(2*y+1) + j)*-1 << endl; clauseNum++;}
		}
	}

	ofile.seekp(6, ios::beg);
	ofile << boardVarShift + (2*x+1)*(2*y+1) << " " << clauseNum;

	
	




	return 0;
}

void slotEdgeCondition(ostream& ofile, edge e, int x, int y, int correntSlotShift, int slotVarShift, int boardVarShift){
			ofile << (slotVarShift + correntSlotShift) * -1 << " " 
				<< (boardVarShift + e.x1 * (2*y+1) + e.y1)*-1 << " "
				<< boardVarShift + e.x2 * (2*y+1) + e.y2 << endl;	
			ofile << (slotVarShift + correntSlotShift) * -1 << " " 
				<< boardVarShift + e.x1 * (2*y+1) + e.y1 << " "
				<< (boardVarShift + e.x2 * (2*y+1) + e.y2)*-1 << endl;
	return;
}
