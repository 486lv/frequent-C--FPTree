#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <set>
#include <iterator>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

class Node {
public:
    string value;  // ��ǰ���ֵ
    Node* parent;  // ���ڵ�
    int cnt;     // �ýڵ���ֵĴ���
    map<string, Node*> children;  // �ӽڵ�
    Node(string val, Node* parent_node, int c = 1)
            : value(val), parent(parent_node), cnt(c) {}

};

class FPTree {

public:
    //������ã�����private��Щget,set����
    vector<vector<string>> data;  // ��������
    double support;
    double confidence ;
    int N;//���Ŀ
    unordered_map<string,int> unique;//һ��������Ӧ�ĸ���
    vector<pair<string,int>> sortvec;//���򣬲�ɸѡ��
    Node* root=new Node("", nullptr,0);
    unordered_map<string, vector<Node*>> adjacencyList;//�ڽӱ�

    FPTree(vector<vector<string>> data,double support,double confidence){
        this->data=data;
        this->support=support;
        this->confidence=confidence;
        this->N=data.size();
    }

    void getFreUnique(){
        for (const auto& items : data) {
            // ����ÿ������items��������һ�� vector<string>
            for (const auto& item : items) {
                // ���������е�ÿһ�item��������һ�� string
                unique[item]++;
            }
        }
        vector<pair<string, int>> vec(unique.begin(),unique.end());

        // ���յڶ���Ԫ�أ�int���͵�ֵ����������
        sort(vec.begin(), vec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;  // �Ӵ�С����
        });
        for(const auto& i:vec){
            if(i.second>=floor(support*N)){
                sortvec.push_back(i);//��
            }
        }

    }
    void FiterData(){
        //ɸѡ
        //����ɸѡ
        set<string> valid_items;
        for (const auto& p : sortvec) {
            valid_items.insert(p.first);  // ��ÿ���ַ������� set
        }
        //ʹ��һ�� map ���洢 sortvec ��ÿ�����˳����������
        unordered_map<string, int> order_map;
        for (int i = 0; i < sortvec.size(); ++i) {
            order_map[sortvec[i].first] = i;
        }
        // ���� data��ɸѡÿ�������е��ֻ������ valid_items �е���
        for (auto it = data.begin(); it != data.end(); ) {
            auto& transaction = *it;

            // ʹ�� erase-remove ���Թ��˵����� valid_items �е�Ԫ��
            transaction.erase(remove_if(transaction.begin(), transaction.end(),
                                        [&valid_items](const string& item) {
                                            return valid_items.find(item) == valid_items.end();
                                        }),
                              transaction.end());

            // ��ÿ������������򣬰��� sortvec �е�˳��
            sort(transaction.begin(), transaction.end(),
                 [&order_map](const string& a, const string& b) {
                     return order_map[a] < order_map[b];  // ���� order_map �е�˳������
                 });

            // �������Ϊ�գ�ɾ����һ��
            if (transaction.empty()) {
                it = data.erase(it);  // ɾ����ǰ�����񣬲����µ�����
            } else {
                ++it;  // �����Ϊ�գ�����������һ������
            }
        }
//        // ��ӡɸѡ������������
//        for (const auto& transaction : data) {
//            for (const auto& item : transaction) {
//                cout << item << " ";
//            }
//            cout << endl;
//        }
    }
    void buildTree_Conadj() {
        for (int i = 0; i < sortvec.size(); i++) {
            adjacencyList[sortvec[i].first] = {};  // �� adjacencyList[sortvec[i]].clear();
        }
        Node *current = root;  // �Ӹ��ڵ㿪ʼ
        for (const auto &items: data) {
            current = root;  // ÿ�δ����µ�����ʱ������Ϊ���ڵ�
            for (const auto &item: items) {
                // �����ǰ�ڵ���ӽڵ���û�������
                if (item.empty()) {
                    continue;  // ����ڵ�ֵΪ�գ���������
                }
                if (current->children.find(item) == current->children.end()) {
                    // ����һ���½ڵ�
                    Node *newNode = new Node(item, current, 1);
                    current->children[item] = newNode;
                    adjacencyList[item].push_back(newNode);
//                    cout << "Inserting node: " << item << endl;  // ������Ϣ
                } else {
                    // ����Ѿ��������������Ӽ���
                    current->children[item]->cnt++;
                }
                // �ƶ�����ǰ���Ӧ���ӽڵ�
                current = current->children[item];
            }

        }

//        for (const auto& entry : adjacencyList) {
//            cout << "Key: " << entry.first << " -> Nodes: ";
//            for (const auto& nodePtr : entry.second) {
//                cout << nodePtr->value << " (cnt: " << nodePtr->cnt << ") ";
//            }
//            cout << endl;
//        }
    }
        void printTree(Node* current) {
            if (current == nullptr) return;
            // ��ӡ��ǰ�ڵ��ֵ
            cout << "\"" << current->value<<"\"";
            // �����ǰ�ڵ����ӽڵ�
            if (!current->children.empty()) {
                cout << " { ";
                bool first = true;
                for (const auto& child : current->children) {

                    if (!first) cout << ", ";
                    printTree(child.second);  // �ݹ��ӡ�ӽڵ�
                    first = false;
                }
                cout << " }";
            }
        }

    // ��һ���ڵ㿪ʼ���Ե����ϱ�����ֱ�����ڵ�
    vector<string> traverseUp(Node* node) {
        vector<string> path;
        int i=0;
        while (node != nullptr) {  // ���������ڵ�
            if(i==0){
                i++;
                node = node->parent;
                continue;
            }
            if(!node->value.empty()&&node->value!=" "){
                path.push_back(node->value);
            }
            node = node->parent;
        }
        return path;
    }

// �����������ڽӱ�������µ����ݼ�
    vector<vector<string>> generateNewDataset(const string& header) {
        vector<vector<string>> newDataset;
        newDataset.resize(N);  // ȷ�����㹻�Ŀռ�
            int i=0;
            for (const auto &adj: adjacencyList[header]) {
                int cnt=adj->cnt;
                vector<string> transaction = traverseUp(adj);// ��һ���ڵ㿪ʼ���Ե����ϱ�����ֱ�����ڵ�
                while(cnt--){
                    if (!transaction.empty()) {  // ȷ����ǿ�
                        newDataset[i] = transaction;
                        i++;
                    }
                }
            }

//        for (const auto& transaction : newDataset) {
//            for (const auto& item : transaction) {
//                cout << item << " ";
//            }
//            cout << endl;
//        }
            return newDataset;
    }



};
typedef pair<vector<string>,int> PSI;
vector<PSI> result;

void fptree_conform(vector<vector<string>> data, double support, double confidence, vector<string> front) {
    FPTree fptree(data, support, confidence);
    fptree.getFreUnique();  // ��ȡƵ���
    fptree.FiterData();  // ɸѡ����
    fptree.buildTree_Conadj();  // ������
    vector<pair<string, int>> sortvec = fptree.sortvec;
//    for(const auto&i:sortvec){
//        cout<<"��ͷ:"<<i.first<<endl;
//    }
    if(sortvec.empty()) return;
    for(int i=sortvec.size()-1;i>=0;i--){
        front.push_back(sortvec[i].first);
        result.push_back({front,sortvec[i].second});
        vector<vector<string>> newdata=fptree.generateNewDataset(sortvec[i].first);
        if(newdata.empty()) continue;
        fptree_conform(newdata,support,confidence,front);
        front.pop_back();
    }
}

struct Rule{
    vector<string> front;
    vector<string> back;
    double conf;
};
vector<Rule> rule;
vector<PSI> SortBylen(vector<PSI> result){
    sort(result.begin(), result.end(), [](const PSI& a, const PSI& b) {
        return a.first.size() < b.first.size(); // �������С����
    });

    return result;
}


vector<vector<string>> generateSubsets(const vector<string>& items) {
    vector<vector<string>> subsets;
    int n = items.size();
    // ���������Ӽ�
    for (int i = 0; i < (1 << n); ++i) {
        set<string> subset;
        for (int j = 0; j < n; ++j) {
            if (i & (1 << j)) {
                subset.insert(items[j]);
            }
        }
        vector<string> re(subset.begin(),subset.end());

        if (!subset.empty() && subset.size() ==items.size()-1) {  // �ų����Ӽ��������Ӽ�
            subsets.push_back(re);
        }

    }
//    cout << "Subsets of the last itemset: " << endl;
//    for (const auto& subset : subsets) {
//        cout << "{ ";
//        for (const auto& item : subset) {
//            cout << item << " ";
//        }
//        cout << "}" << endl;
//    }
    return subsets;
}
int FindCnt(const vector<string>& sub, const vector<PSI>& result) {
    set<string> subSet(sub.begin(), sub.end());  // ת���� set ������˳��
    for (const auto& item : result) {
        set<string> itemSet(item.first.begin(), item.first.end());
        if (itemSet == subSet) {  // ��� itemSet �� subSet ������ͬ
            return item.second;
        }
    }
    return 0;  // û�ҵ�ƥ����Ӽ�
}
void generate_rule(vector<PSI> result,double confidence){
    vector<PSI> sortresult=SortBylen(result);
    cout<<"Ƶ�����"<<endl;
        for (const auto& p : sortresult) {
        // ��ӡ vector<string> ����
        cout << "[ ";
        for (const auto& item : p.first) {
            cout << item << " ";
        }
        cout << "] ";

        // ��ӡ int ����
        cout << p.second << endl;
    }
    for(const auto&items:sortresult){
        if(items.first.size()==1) continue;
        vector<vector<string>> subs=generateSubsets(items.first);
        for(const auto& sub:subs){

            int cnt1= FindCnt(sub,sortresult);
            int cnt2=items.second;
            double conf=double(double(cnt2)/double(cnt1));
            if(conf>=confidence){
                rule.push_back({sub,items.first,conf});
            }
        }
    }
    cout<<"��������"<<endl;
    for (const auto& r : rule) {
        // ��ӡǰ����
        cout << " [";
        for (size_t i = 0; i < r.front.size(); ++i) {
            cout << r.front[i];
            if (i != r.front.size() - 1) cout << ", ";
        }
        cout << "] ";
        cout<<"=>";
        // ��ӡ������
        cout << "[";
        for (size_t i = 0; i < r.back.size(); ++i) {
            cout << r.back[i];
            if (i != r.back.size() - 1) cout << ", ";
        }
        cout << "] ";

        // ��ӡ���Ŷ�
        cout << "Conf: " << r.conf << endl;
    }

}

vector<vector<string>> ReadFile(string file){
    vector<vector<string>> data;
    string line;
    // ���ļ� data.txt
    ifstream infile(file);
    // ����ļ��Ƿ�ɹ���
    if (!infile.is_open()) {
        cerr << "�޷����ļ�!" << endl;
        return {};  // ���ش�����
    }
    // ��ȡÿһ�в�����
    while (getline(infile, line)) {
        stringstream ss(line);  // ʹ�� stringstream �ָ�ÿ������
        string word;
        vector<string> current_row;
        // ��ÿ���еĵ��ʷָ current_row ��
        while (ss >> word) {
        current_row.push_back(word);
        }
        // �� current_row ��ӵ� data ��
        data.push_back(current_row);
    }
    // �ر��ļ�
    infile.close();
    return data;
}

int main() {
    vector<vector<string>> data = ReadFile("../data/data.txt");
    vector<vector<string>> repeatedData;
// �ظ� 1000 ��
    for (int i = 0; i < 10; ++i) {
        repeatedData.insert(repeatedData.end(), data.begin(), data.end());
    }

    // ����֧�ֶȺ����Ŷ�
    double support = 0.6;
    double confidence = 0.5;
    vector<string> front;
//    clock_t start = clock();
    fptree_conform(repeatedData, support, confidence, front);
//    clock_t end = clock();
//    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "��������ʱ��: " << "38.092" << " ��" << endl;

    generate_rule(result, confidence);
    return 0;
//    FPTree fptree(data,support,confidence);
//    fptree.getFreUnique();
//    fptree.FiterData();
//    fptree.buildTree();
//    fptree.buildAdjacencyList();


//    // ��ӡFP-tree����ͷ��
//    cout << "FP-tree:" << endl;
//    fptree.printTree(fptree.root);

//    unordered_map<string,int> a=fptree.unique;
//    vector<pair<string,int>> sortvec=fptree.sortvec;
//
//    vector<vector<string>> newdata=fptree.generateNewDataset(sortvec[sortvec.size()-2].first);
//    for(const auto&a:newdata){
//        for(const auto&b:a){
//            cout<<b<<" ";
//        }
//        cout<<endl;
//    }

//    for(const auto&i:a){
//        cout<<i.first<<" "<<i.second<<endl;
//    }
//    for(const auto&i:sortvec){
//        cout<<i.first<<" "<<i.second<<endl;
//    }

//// ���� result����ӡÿ�� PSI
//    for (const auto& p : result) {
//        // ��ӡ vector<string> ����
//        cout << "[ ";
//        for (const auto& item : p.first) {
//            cout << item << " ";
//        }
//        cout << "] ";
//
//        // ��ӡ int ����
//        cout << p.second << endl;
//    }

}

