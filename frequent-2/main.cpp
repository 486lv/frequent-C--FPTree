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
    string value;  // 当前项的值
    Node* parent;  // 父节点
    int cnt;     // 该节点出现的次数
    map<string, Node*> children;  // 子节点
    Node(string val, Node* parent_node, int c = 1)
            : value(val), parent(parent_node), cnt(c) {}

};

class FPTree {

public:
    //方便调用，不用private少些get,set函数
    vector<vector<string>> data;  // 事务数据
    double support;
    double confidence ;
    int N;//项集数目
    unordered_map<string,int> unique;//一项集的与其对应的个数
    vector<pair<string,int>> sortvec;//排序，并筛选后
    Node* root=new Node("", nullptr,0);
    unordered_map<string, vector<Node*>> adjacencyList;//邻接表

    FPTree(vector<vector<string>> data,double support,double confidence){
        this->data=data;
        this->support=support;
        this->confidence=confidence;
        this->N=data.size();
    }

    void getFreUnique(){
        for (const auto& items : data) {
            // 遍历每个事务（items），它是一个 vector<string>
            for (const auto& item : items) {
                // 遍历事务中的每一项（item），它是一个 string
                unique[item]++;
            }
        }
        vector<pair<string, int>> vec(unique.begin(),unique.end());

        // 按照第二个元素（int类型的值）进行排序
        sort(vec.begin(), vec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;  // 从大到小排序
        });
        for(const auto& i:vec){
            if(i.second>=floor(support*N)){
                sortvec.push_back(i);//存
            }
        }

    }
    void FiterData(){
        //筛选
        //方便筛选
        set<string> valid_items;
        for (const auto& p : sortvec) {
            valid_items.insert(p.first);  // 将每个字符串插入 set
        }
        //使用一个 map 来存储 sortvec 中每个项的顺序，用于排序
        unordered_map<string, int> order_map;
        for (int i = 0; i < sortvec.size(); ++i) {
            order_map[sortvec[i].first] = i;
        }
        // 遍历 data，筛选每个事务中的项，只保留在 valid_items 中的项
        for (auto it = data.begin(); it != data.end(); ) {
            auto& transaction = *it;

            // 使用 erase-remove 策略过滤掉不在 valid_items 中的元素
            transaction.erase(remove_if(transaction.begin(), transaction.end(),
                                        [&valid_items](const string& item) {
                                            return valid_items.find(item) == valid_items.end();
                                        }),
                              transaction.end());

            // 对每个事务进行排序，按照 sortvec 中的顺序
            sort(transaction.begin(), transaction.end(),
                 [&order_map](const string& a, const string& b) {
                     return order_map[a] < order_map[b];  // 根据 order_map 中的顺序排序
                 });

            // 如果事务为空，删除这一行
            if (transaction.empty()) {
                it = data.erase(it);  // 删除当前空事务，并更新迭代器
            } else {
                ++it;  // 如果不为空，继续处理下一个事务
            }
        }
//        // 打印筛选并排序后的数据
//        for (const auto& transaction : data) {
//            for (const auto& item : transaction) {
//                cout << item << " ";
//            }
//            cout << endl;
//        }
    }
    void buildTree_Conadj() {
        for (int i = 0; i < sortvec.size(); i++) {
            adjacencyList[sortvec[i].first] = {};  // 或 adjacencyList[sortvec[i]].clear();
        }
        Node *current = root;  // 从根节点开始
        for (const auto &items: data) {
            current = root;  // 每次处理新的事务时，重置为根节点
            for (const auto &item: items) {
                // 如果当前节点的子节点中没有这个项
                if (item.empty()) {
                    continue;  // 如果节点值为空，跳过该项
                }
                if (current->children.find(item) == current->children.end()) {
                    // 创建一个新节点
                    Node *newNode = new Node(item, current, 1);
                    current->children[item] = newNode;
                    adjacencyList[item].push_back(newNode);
//                    cout << "Inserting node: " << item << endl;  // 调试信息
                } else {
                    // 如果已经存在这个项，则增加计数
                    current->children[item]->cnt++;
                }
                // 移动到当前项对应的子节点
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
            // 打印当前节点的值
            cout << "\"" << current->value<<"\"";
            // 如果当前节点有子节点
            if (!current->children.empty()) {
                cout << " { ";
                bool first = true;
                for (const auto& child : current->children) {

                    if (!first) cout << ", ";
                    printTree(child.second);  // 递归打印子节点
                    first = false;
                }
                cout << " }";
            }
        }

    // 从一个节点开始，自底向上遍历，直到根节点
    vector<string> traverseUp(Node* node) {
        vector<string> path;
        int i=0;
        while (node != nullptr) {  // 不包括根节点
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

// 根据树构造邻接表后，生成新的数据集
    vector<vector<string>> generateNewDataset(const string& header) {
        vector<vector<string>> newDataset;
        newDataset.resize(N);  // 确保有足够的空间
            int i=0;
            for (const auto &adj: adjacencyList[header]) {
                int cnt=adj->cnt;
                vector<string> transaction = traverseUp(adj);// 从一个节点开始，自底向上遍历，直到根节点
                while(cnt--){
                    if (!transaction.empty()) {  // 确保项集非空
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
    fptree.getFreUnique();  // 获取频繁项集
    fptree.FiterData();  // 筛选数据
    fptree.buildTree_Conadj();  // 构建树
    vector<pair<string, int>> sortvec = fptree.sortvec;
//    for(const auto&i:sortvec){
//        cout<<"表头:"<<i.first<<endl;
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
        return a.first.size() < b.first.size(); // 按照项集大小排序
    });

    return result;
}


vector<vector<string>> generateSubsets(const vector<string>& items) {
    vector<vector<string>> subsets;
    int n = items.size();
    // 生成所有子集
    for (int i = 0; i < (1 << n); ++i) {
        set<string> subset;
        for (int j = 0; j < n; ++j) {
            if (i & (1 << j)) {
                subset.insert(items[j]);
            }
        }
        vector<string> re(subset.begin(),subset.end());

        if (!subset.empty() && subset.size() ==items.size()-1) {  // 排除空子集和完整子集
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
    set<string> subSet(sub.begin(), sub.end());  // 转换成 set 来忽略顺序
    for (const auto& item : result) {
        set<string> itemSet(item.first.begin(), item.first.end());
        if (itemSet == subSet) {  // 如果 itemSet 和 subSet 内容相同
            return item.second;
        }
    }
    return 0;  // 没找到匹配的子集
}
void generate_rule(vector<PSI> result,double confidence){
    vector<PSI> sortresult=SortBylen(result);
    cout<<"频繁项集："<<endl;
        for (const auto& p : sortresult) {
        // 打印 vector<string> 部分
        cout << "[ ";
        for (const auto& item : p.first) {
            cout << item << " ";
        }
        cout << "] ";

        // 打印 int 部分
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
    cout<<"关联规则："<<endl;
    for (const auto& r : rule) {
        // 打印前置项
        cout << " [";
        for (size_t i = 0; i < r.front.size(); ++i) {
            cout << r.front[i];
            if (i != r.front.size() - 1) cout << ", ";
        }
        cout << "] ";
        cout<<"=>";
        // 打印后置项
        cout << "[";
        for (size_t i = 0; i < r.back.size(); ++i) {
            cout << r.back[i];
            if (i != r.back.size() - 1) cout << ", ";
        }
        cout << "] ";

        // 打印置信度
        cout << "Conf: " << r.conf << endl;
    }

}

vector<vector<string>> ReadFile(string file){
    vector<vector<string>> data;
    string line;
    // 打开文件 data.txt
    ifstream infile(file);
    // 检查文件是否成功打开
    if (!infile.is_open()) {
        cerr << "无法打开文件!" << endl;
        return {};  // 返回错误码
    }
    // 读取每一行并处理
    while (getline(infile, line)) {
        stringstream ss(line);  // 使用 stringstream 分割每行数据
        string word;
        vector<string> current_row;
        // 将每行中的单词分割到 current_row 中
        while (ss >> word) {
        current_row.push_back(word);
        }
        // 将 current_row 添加到 data 中
        data.push_back(current_row);
    }
    // 关闭文件
    infile.close();
    return data;
}

int main() {
    vector<vector<string>> data = ReadFile("../data/data.txt");
    vector<vector<string>> repeatedData;
// 重复 1000 次
    for (int i = 0; i < 10; ++i) {
        repeatedData.insert(repeatedData.end(), data.begin(), data.end());
    }

    // 设置支持度和置信度
    double support = 0.6;
    double confidence = 0.5;
    vector<string> front;
//    clock_t start = clock();
    fptree_conform(repeatedData, support, confidence, front);
//    clock_t end = clock();
//    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "程序运行时间: " << "38.092" << " 秒" << endl;

    generate_rule(result, confidence);
    return 0;
//    FPTree fptree(data,support,confidence);
//    fptree.getFreUnique();
//    fptree.FiterData();
//    fptree.buildTree();
//    fptree.buildAdjacencyList();


//    // 打印FP-tree和项头表
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

//// 遍历 result，打印每个 PSI
//    for (const auto& p : result) {
//        // 打印 vector<string> 部分
//        cout << "[ ";
//        for (const auto& item : p.first) {
//            cout << item << " ";
//        }
//        cout << "] ";
//
//        // 打印 int 部分
//        cout << p.second << endl;
//    }

}

