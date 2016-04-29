

void readData(Packet p){
	//something bout getting k and putting destinations into an array, possible Dest, if they are not 00000000.00000000
}

void chooseKdest(int k, possibleHops){
	if (k==3){
		findNextHop(k,possibleHops);
	}
	if (k==2){
		int max = possibleHops[0]; // Figure out if max should be an index of possibleHops or the hop itself
		for (int i = 1; i < D.size(); i++){
			if (possibleHops[i].cost > max.cost){
				max = possibleHops[i];
			}
		}
		//remove D[i] from possibleHops
		findNextHop(k,possibleHops);
	}
	if (k==1){
		int least = possibleHops[0];
		for (int i = 1; i<possibleRoutes.size(); i++){
			if (possibleHops.cost < least.cost){
				least = possibleHops[i];
			}
		}
		forwardPacket();
	}
}

void findNextHop(int k, possibleHops){
	//Create default nextHops from first choices
	std::array<int>[k][1] forwardTo;
	for (int i = 0; i<k; i++){
		forwardTo = possibleRoutes[i][0];
	}

	//Finding common nextHops
	for (int i1 = 0; i1 < k-1; i1++){
		for (int j1 = i1+1; j1<k; j++){
			//Only check for a common nextHop if the current choices are different
			if (forwardTo[i1] != forwardTo[j1]){
				for (int i2 = 0; i2<possibleRoutes[i1].size(); i2++ ){
					for (int j2 = 0; possibleRoutes[j1].size(); j2++){
						if (possibleRoutes[i1][i2] == possibleRoutes[j1][j2]){
							forwardTo[i] == possibleRoutes[i][i2];
							forwardTo[j] = forwardTo[i];
							break;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < forwardTo.size(); i++){
		std::array<int> erasedDest = [1, 1, 1];
		if (forwardTo[i] != 0){
			int currentHop = forwardTo[i];
			forwardTo[i] = 0; //null the entry to skip over it later
			erasedDest[i] = 0;
			for (int j = i; j <forwardTo.size(); j++){
				if (currentHop == forwardTo[j]){
					forwardTo[i] = 0; //null the entry to skip over it later
					erasedDest[i] = 0;
				}
			}
			int new_k = k;
			for (int i = 0; i < erasedDest.size(); i++}{
				new_k = new_k - erasedDest[i];
			}
			forwardPacket(p,new_k,erasedDest,currentHop);
		}
		

		
	}
	
}

void forwardPacket(Packet p, int k, erasedDest, nextHop){
	//Needs to copy packets then somehow erase the D's
}