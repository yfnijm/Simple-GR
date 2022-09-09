#include<iostream>


class Config{
public:
	init();

	int width, height;
	int net_num;
	vecotr<int> x_coor;
	vecotr<int> y_coor;

};


void init();

int main(){
	Config config = init();


}


Config::init(){
	cout << "Grid size:" << endl;
	cin >> width >> height;
	
	cout << "Pin Num:" << endl;
	cin >> net_num;

	x_coor.resize(net_num);
	y_coor.resize(net_num);

	for(int i = 0; i < net_num; i++){
		cin >> x_coor[i] >> y_coor[i];
	}

	

}






