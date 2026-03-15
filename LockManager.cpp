#include "httplib.h"
#include<iostream>
#include<mutex>
#include<vector>
#include<unordered_map>

using namespace std;
class TreeNode{
    public:
    int id;
    bool islocked;
    int lockedby;
    int lockdesendent_count;
    TreeNode* parent;
    vector<TreeNode*>children;
    TreeNode(int _id) {
        id = _id;
        islocked = false;
        lockedby = -1;
        lockdesendent_count = 0;
        parent = nullptr;
    }
};

class accessmanager{
    unordered_map<int,TreeNode*>nodemap;
    TreeNode* root;
    std::recursive_mutex tree_mutex;
    public:
    accessmanager(vector<int>&id,int m){
        for(int ids: id){
            nodemap[ids]=new TreeNode(ids);
        }
        root=nodemap[id[0]];
        for(int i=0;i<id.size();i++){
            TreeNode* parentnode=nodemap[id[i]];
            for(int j=1;j<=m;j++){
                int childinx=i*m+j;
                if(childinx<id.size()){
                    TreeNode* childnode=nodemap[id[childinx]];
                    parentnode->children.push_back(childnode);
                    childnode->parent=parentnode;
                }
            }
        }
    }
    bool lock(int id,int uid){
        std::lock_guard<std::recursive_mutex> guard(tree_mutex);
        TreeNode* node=nodemap[id];
        if(node->islocked==true) return false;
        if(node->lockdesendent_count>0) return false;
        TreeNode* temp=node;
        while(temp!=nullptr){
          
            if(temp->islocked==true) return false;
              temp=temp->parent;
        }
        node->islocked=true;
        node->lockedby=uid;
        TreeNode* temp2=node->parent;
        
        while(temp2!=nullptr){
        temp2->lockdesendent_count++;
        temp2=temp2->parent;
    }
    return true;
        
    }
    bool unlock(int id,int uid){
        std::lock_guard<std::recursive_mutex> guard(tree_mutex);
        TreeNode*node1=nodemap[id];
        if(node1->islocked==false) return false;
        if(node1->lockedby!=uid) return false;
        if(node1->islocked==true && node1->lockedby==uid){
            node1->islocked=false;
            TreeNode*temp2=node1;
            temp2=temp2->parent;
            while(temp2!=nullptr){
                temp2->lockdesendent_count--;
                temp2=temp2->parent;
            }
            return true;
            }
            return false; 
    }
    void dfs(TreeNode* temp,bool & flag,int uid,vector<TreeNode*> & target){
        if(flag==false) return;
        if(temp->islocked==true && temp->lockedby==uid){
            target.push_back(temp);
        }
        else if(temp->islocked==true && temp->lockedby!=uid){
            flag=false;
            return;
        }
        vector<TreeNode*>k=temp->children;
        for(int i=0;i<k.size();i++){
            dfs(k[i],flag,uid,target);
        }
    }
    bool upgrade(int id,int uid){
        std::lock_guard<std::recursive_mutex> guard(tree_mutex);
        TreeNode* temp=nodemap[id];
        vector<TreeNode*>target;
        bool flag=true;
        dfs(temp,flag,uid,target);
        if(flag==false) return false;
        else{
            for(int i=0;i<target.size();i++){
                TreeNode* temp=target[i];
                temp->islocked=false;
                temp=temp->parent;
                while(temp!=nullptr){
                    temp->lockdesendent_count--;
                    temp=temp->parent;
                }
            }
            temp=nodemap[id];
            temp->islocked=true;
            temp->lockedby=uid;
            temp=temp->parent;
            while(temp!=nullptr){
                temp->lockdesendent_count++;
                temp=temp->parent;
            }
            return true;

        }
    }

};


int main() {
    cout << "Initializing Tree..." << endl;
    vector<int> ids = {1, 2, 3, 4, 5, 6, 7};
    accessmanager tree(ids, 2); 

    httplib::Server svr;

    // API ENDPOINT 1: /lock?id=4&uid=1
    svr.Get("/lock", [&tree](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id") && req.has_param("uid")) {
            int id = stoi(req.get_param_value("id"));
            int uid = stoi(req.get_param_value("uid"));
            
            bool success = tree.lock(id, uid);
            
            if (success) res.set_content("SUCCESS: Node " + to_string(id) + " locked by " + to_string(uid) + "\n", "text/plain");
            else res.set_content("FAIL: Could not lock node\n", "text/plain");
        } else {
            res.set_content("ERROR: Missing id or uid parameters\n", "text/plain");
        }
    });

    // API ENDPOINT 2: /unlock?id=4&uid=1
    svr.Get("/unlock", [&tree](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id") && req.has_param("uid")) {
            int id = stoi(req.get_param_value("id"));
            int uid = stoi(req.get_param_value("uid"));
            
            bool success = tree.unlock(id, uid);
            
            if (success) res.set_content("SUCCESS: Node unlocked\n", "text/plain");
            else res.set_content("FAIL: Could not unlock node\n", "text/plain");
        }
    });

    // API ENDPOINT 3: /upgrade?id=2&uid=1
    svr.Get("/upgrade", [&tree](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id") && req.has_param("uid")) {
            int id = stoi(req.get_param_value("id"));
            int uid = stoi(req.get_param_value("uid"));
            
            bool success = tree.upgrade(id, uid);
            
            if (success) res.set_content("SUCCESS: Node upgraded\n", "text/plain");
            else res.set_content("FAIL: Could not upgrade node\n", "text/plain");
        }
    });

    cout << "=======================================" << endl;
    cout << "🚀 API Server is LIVE on port 8080!" << endl;
    cout << "Try opening Chrome and going to:" << endl;
    cout << "http://localhost:8081/lock?id=4&uid=1" << endl;
    cout << "=======================================" << endl;
    
  svr.listen("localhost", 9999);

    return 0;
}