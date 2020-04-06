 /* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <cstdlib>
#include <string>
#include <bitset>

using namespace std;


static bool comparator1(pair<string,pair<int,int>> A,pair<string,pair<int,int>> B){
    return A.second.first > B.second.first;
}
static bool comparator2(pair<string,pair<int,int>> A,pair<string,pair<int,int>> B){
    if(A.second.first==B.second.first){
        return A.second.second < B.second.second;
    }
    return A.second.first>B.second.first;
}
static bool comparator3(pair<int,string> A,pair<int,string> B){
    return A.first < B.first;
}


vector<string> reader(string filename){
    ifstream infile(filename);
    vector<string> V;
    string temp;
    while(infile>>temp){
        V.push_back(temp);
    }
    infile.close();
    return V;
}

enum types {RLE,BBC,oneMismatch,twoCMismatch,twoAMismatch,directMatch,OriginalBinaries};

//compression id's
vector<string>  cid = {"000","001","010","011","100","101","110"};
vector<int> WLE = {5,15,11,11,16,6,35};

bool oneMismatches(string A,string B){
    int counter = 0;
    for(int i=0;i<A.length();i++){
        if(A[i]!=B[i]){counter++;}
    }
    return counter==1;
}
int oneMismatchesPos(string A,string B){
    for(int i=0;i<A.length();i++){
        if(A[i]!=B[i]){return i;}
    }
    // It should not go here;
    return -1;
}
// consecutive mismatches
bool twoCMismatches(string A,string B){
    int counter = 0;
    vector<int> pos;
    for(int i=0;i<A.length();i++){
        if(A[i]!=B[i]){
            pos.push_back(i);
        }
    }

    if(pos.size()==2 and pos[0]+1==pos[1]){return 1;}
    return 0;
}

bool twoAnyMismatches(string A,string B){
    int counter = 0;
    vector<int> mismatches;
    for(int i=0;i<A.length();i++){
        if(A[i]!=B[i]){
            counter++;
            mismatches.push_back(i);
        }
    }
    if(mismatches.size()>2){return 0;}
    else if(mismatches.size()==2){
        return 1;
    }
    return 0;
}
bool bbc(string A,string B){
    bitset<32> Abs(A);
    bitset<32> Bbs(B); 
    int counter = 0;
    for(int i=0;i<A.size()-3;i++){
        bitset<4> Am(A, i, 4);
        bitset<4> Bm(B, i, 4);
        for(int i=0;i<4;i++){
            Bm[i] = Bm[i] ^ Am[i];
        }
        bitset<32> temp(B);
        for(int j=0;j<4;j++){
            temp[28 - i+j] = temp[28 - i+j] ^ Bm[j];
        }
        //For debugging purposes
       // if(temp==Abs){
        //     cout<<"Mask: "<<Bm.to_string()<<" Starting Position:"<<i<<endl;
        // }
        if(temp==Abs){
            counter++;
        }
    }
    if(counter>0){
        return 1;
    }
    return 0;
}

pair<string,string> bbcpos(string A,string B){
    bitset<32> Abs(A);
    bitset<32> Bbs(B); 
    int counter = 0;
    vector<int> position;
    vector<string> masks;
    for(int i=0;i<A.size()-3;i++){
        bitset<4> Am(A, i, 4);
        bitset<4> Bm(B, i, 4);
        for(int i=0;i<4;i++){
            Bm[i] = Bm[i] ^ Am[i];
        }
        bitset<32> temp(B);
        for(int j=0;j<4;j++){
            temp[28 - i+j] = temp[28 - i+j] ^ Bm[j];
        }
        if(temp==Abs){
            position.push_back(i);
            masks.push_back(Bm.to_string());
        }
    }
    int i = 0;
    while(1){
        if(masks[i][0]=='1'){
            break;
        }
        i++;
    }
    string s = bitset<5>(position[i]).to_string();
    return {s,masks[i]}; // {starting location , bitmask}

}




pair<int,int> twoAnyMismatchesPos(string A,string B){
        vector<int> mismatches;
        for(int i=0;i<A.length()-1;i++){
        if(A[i]!=B[i]){
            mismatches.push_back(i);
        }
    }
    return {mismatches[0],mismatches[1]};
}

int twoMismatchesPos(string A,string B){
        for(int i=0;i<A.length()-1;i++){
          if(A[i]!=B[i] and A[i+1]!=B[i+1]){
                return i;
        }
    }
    //It should not go here;
    return -1;
}

map<string,int> dictionaryBuilder(vector<string> words){
    map<string,pair<int,int>> counter; // count and first occurrence
    for(int i=0;i<words.size();i++){
        if(counter.find(words[i])==counter.end()){
            pair<int,int> temp = {1,i};
            counter[words[i]] = temp; 
        }
        else{
            auto t = counter[words[i]];
            t.first += 1;
            counter[words[i]] = t;

        }
    }
    // Sort it and convert into map<string,string>
    vector<pair<string,pair<int,int>>> V;
    for(auto it = counter.begin();it!=counter.end();it++){
        pair<string,pair<int,int>> T;
        pair<int,int> t2 = {it->second.first,it->second.second};
        T.first = it->first;
        T.second = t2;
        V.emplace_back(T);
    }
    sort(V.begin(),V.end(),comparator1);
    sort(V.begin(),V.end(),comparator2);


    //For debugging
    // for(auto it:V){
    //     cout<<it.first<<"    "<<it.second.first<<" "<<it.second.second<<endl;
    // }



    map<string,int> dictionary;
    for(int i=0;i<8;i++){
        dictionary[V[i].first] = i;
    }
    return dictionary;
    
}

//Dictionary based on bit values
map<string,string> dictCoverter(map<string,int> dictionary){
    map<string,string> M;
    for(auto it=dictionary.begin();it!=dictionary.end();it++){
        M[bitset<3>(it->second).to_string()] = it->first;
    }
    return M;
}

map<int,string> dictCoverter2(map<string,int> dictionary){
    map<int,string> M;
    for(auto it=dictionary.begin();it!=dictionary.end();it++){
        M[it->second] = it->first;
    }
    return M;
}



pair<int,int> checker(string A,map<int,string> dictionary,map<string,int> dict2){
    int option = 6;
    auto it = dictionary.begin();
    vector<pair<int,string>> ops;
    if(dict2.find(A)!=dict2.end()){
        return {5,dict2[A]};
    }
    for(;it!=dictionary.end();it++){
        if(A==it->second){
            ops.emplace_back(5,it->second);
        }
        else if(oneMismatches(A,it->second) and twoCMismatches(A,it->second)){
            ops.emplace_back(2,it->second);
        }
        else if(oneMismatches(A,it->second)){
            ops.emplace_back(2,it->second);
        }
        else if(twoCMismatches(A,it->second)){
            ops.emplace_back(3,it->second);
        }
        else if(bbc(A,it->second)){
            ops.emplace_back(1,it->second);

        }
        else if(twoAnyMismatches(A,it->second)){
            ops.emplace_back(4,it->second);
        }        
    }
    if(ops.size()==0){
        return {option,it->first};
    }
    sort(ops.begin(),ops.end(),comparator3);
    int score = WLE[0];
    option = ops[0].first;
    int index = 0;
    for(int i=1;i<ops.size();i++){
        if(WLE[ops[i].first]<score){
            score = WLE[ops[i].first];
            option = ops[i].first;
            index = i;
        }
    }
    
    return {option,dict2[ops[index].second]}; //option, index
}

//Print the bit string
string printer(string A,map<int,string> dictionary,map<string,int> dict2){
    pair<int,int> option = checker(A,dictionary,dict2);
    string compressed_string = "";
     if(option.first==1){
            pair<string,string> temp = bbcpos(A,dictionary[option.second]);
           compressed_string=cid[option.first]+temp.first+temp.second+bitset<3>(option.second).to_string();
            }
    else if(option.first==2){
            int t = oneMismatchesPos(A,dictionary[option.second]);
            compressed_string=cid[option.first]+bitset<5>(t).to_string()+bitset<3>(option.second).to_string();
    }
    else if(option.first==3){
        int t = twoMismatchesPos(A,dictionary[option.second]);
        compressed_string=cid[option.first]+bitset<5>(t).to_string()+bitset<3>(option.second).to_string();
    }
    else if(option.first==4){
        pair<int,int> t = twoAnyMismatchesPos(A,dictionary[option.second]);
        compressed_string=cid[option.first]+bitset<5>(t.first).to_string()+bitset<5>(t.second).to_string()+bitset<3>(option.second).to_string();
    }
    else if(option.first==5){
        compressed_string=cid[option.first]+bitset<3>(option.second).to_string();
    }
    else if(option.first==6){
        compressed_string=cid[option.first]+A;
    }
    return compressed_string;
    }

vector<string> phase2(vector<string> phase1){
    vector<string> p2;
    for(int i=0;i<phase1.size();i++){
        string current = phase1[i];
        int temp = i+1;
        int counter = 0;
        for(int j=i+1,k=0; j<phase1.size() and k<8;j++,k++){
            if(phase1[j]==current){counter++;i=j;}
            else{break;}
        }
        if(counter>0){p2.push_back(current);p2.emplace_back("000"+bitset<2>(counter-1).to_string());}
        else{p2.push_back(current);}
    }
    return p2;
}
void compress(string input){
    vector<string> words = reader(input);
    map<string,int> t = dictionaryBuilder(words);
    map<int,string> tt = dictCoverter2(t);
    map<string,string> ttt = dictCoverter(t);
    vector<string> phase1;
    for(auto it:words){
        phase1.push_back(printer(it,tt,t));
    }

    vector<string> p2 = phase2(phase1);
    string temp = "";
    for(auto it:p2){
        temp+=it;
    }

    vector<string> ff;
    for (size_t i = 0; i < temp.size(); i += 32){
        string t = temp.substr(i, 32);
        while(t.size()<32){
            t+="1";
        }
        ff.push_back(t);
    }


   ofstream outfile;
   outfile.open("cout.txt");

    //write them in a file.

    for(auto t:ff){
        outfile<<t<<endl;
    }

    outfile<<"xxxx"<<endl;
    //Write the dictionary

    for(auto it=tt.begin();it!=tt.end();it++){
        outfile<<it->second<<endl;
    }

    outfile.close();

}



// vector<string> reader(string filename){
//     ifstream infile(filename);
//     vector<string> V;
//     string temp;
//     while(infile>>temp){
//         V.push_back(temp);
//     }
//     infile.close();
//     return V;
// }


void decompress(string input){
    //Create a large string;
    //Create a dictionary;
    map<string,string> dict;
    string large = "";
    ifstream infile("compressed.txt");
    string temp;
    //Repair the getline


    while(infile>>temp)
    {    
        if(temp=="xxxx"){
            break;
        }
        large+=temp;
    }
    for(int i=0;infile>>temp;i++){
        dict[bitset<3>(i).to_string()] = temp;
    }
   
    int i = 0;
    string prev_instruction = "";
    vector<string> orgiinst;
    while(1){
        string id = large.substr(i,3);
        i=i+3;
        if(id=="000"){
            int n = stoi(large.substr(i,2),0,2)+1;
            for(int j=0;j<n;j++){
                orgiinst.push_back(prev_instruction);
            }
            i = i+2;
        }
        else if(id=="001"){
            string inst = large.substr(i,12);
            i = i+12;
            string sloc = inst.substr(0,5);
            string mask = inst.substr(5,4);
            string index = inst.substr(9,3);
            string dictvalue = dict[index];
            // cout<<"SLOC: "<<sloc<<" Mask: "<<mask<<" index "<<index<<endl;
            // cout<<"conversion: "<<stoi(sloc,0,2)<<endl;
            for(int j = stoi(sloc,0,2),count=0;count<4;j++,count++){
                if(mask[count]==dictvalue[j]){
                    dictvalue[j] = '0';
                }
                else{
                    dictvalue[j] = '1';

                }
            }
                prev_instruction = dictvalue;
                orgiinst.push_back(prev_instruction);
            }
        else if(id=="010"){
            string inst = large.substr(i,8);
            i+=8;
            string mismatchloc = inst.substr(0,5);
            int loc = stoi(mismatchloc,0,2);
            string index = inst.substr(5,3);
            string dictvalue = dict[index];
            if(dictvalue[loc]=='1'){
                dictvalue[loc] = '0';
            }
            else{
                dictvalue[loc] = '1';
            }
            prev_instruction = dictvalue;
            orgiinst.push_back(prev_instruction);

        }
        else if(id=="011"){
            string inst = large.substr(i,8);
            i+=8;
            string mismatchloc = inst.substr(0,5);
            int loc = stoi(mismatchloc,0,2);
            string index = inst.substr(5,3);
            string dictvalue = dict[index];
            if(dictvalue[loc]=='1'){
                dictvalue[loc] = '0';
            }
            else{
                dictvalue[loc] = '1';
            }
            if(dictvalue[loc+1]=='1'){
                dictvalue[loc+1] = '0';
            }
            else{
                dictvalue[loc+1] = '1';
            }
            prev_instruction = dictvalue;
            orgiinst.push_back(prev_instruction);
        }
        else if(id=="100"){
            string inst = large.substr(i,13);
            i+=13;
            string mismatchloc1 = inst.substr(0,5);
            string mismatchloc2 = inst.substr(5,5);
            int loc1 = stoi(mismatchloc1,0,2);
            int loc2 = stoi(mismatchloc2,0,2);
            string index = inst.substr(10,3);
            string dictvalue = dict[index];
            if(dictvalue[loc1]=='1'){
                dictvalue[loc1] = '0';
            }
            else{
                dictvalue[loc1] = '1';
            }
            if(dictvalue[loc2]=='1'){
                dictvalue[loc2] = '0';
            }
            else{
                dictvalue[loc2] = '1';
            }
            prev_instruction = dictvalue;
            orgiinst.push_back(prev_instruction);
        }
        else if(id=="101"){
            string index = large.substr(i,3);
            i=i+3;
            string dictvalue = dict[index];
            prev_instruction = dictvalue;
            orgiinst.push_back(prev_instruction);
        }
        else if(id=="110"){
            string inst = large.substr(i,32);
            i = i+32;
            prev_instruction = inst;
            orgiinst.push_back(prev_instruction);
        }
        else if(id=="111" or abs(i-(int)large.size())<3){
            break;
        }

    }
    //Write to a file
    
   ofstream outfile;
   outfile.open("dout.txt");

    for(auto it:orgiinst){
        outfile<<it<<endl;
    }
 	
    outfile.close();

}




int main(int argc,char* argv[]){
    if(argc==1){
	cout<<"Please Pass the Input option"<<endl;
     }
     else if(atoi(argv[1])==1){
	compress("original.txt");
      }
     else if(atoi(argv[1])==2){
	decompress("compressed.txt");
     }
     else{
	cout<<"Invalid Input"<<endl;
     }




    return 0;
}


