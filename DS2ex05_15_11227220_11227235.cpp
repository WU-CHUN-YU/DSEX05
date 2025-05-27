// 資訊二乙 11227220張季平、11227235吳君友
#include <iostream>
#include <fstream>
#include <iomanip>  // setw()
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <utility>  // std::pair
#include <map>
#include <unordered_map>

using ReceiveIDType = std::map<std::string, float>;

// Bin檔中的資料型態
struct DataType {
  std::array<char, 12> send_id{};     // 發訊者學號
  std::array<char, 12> receive_id{};  // 收訊者學號
  float weight;                       // 量化權重
};


class AdjacencyLists {
 public:
  std::map<std::string, ReceiveIDType> adjacency_list;  // adjcency_list 相鄰串列
  // ans_list 答案串列(連通點陣列)
 public:
 
  void SortAdjcencyList() {
    // 將任務零陣列 改為以連通數由大到小排列
  }

  int CalculateConnectionNumber() {
    /*
    sudo code:
    資料:
    已完成的檢索表
    接收者堆疊
    std::stack<std::string> recipient;
    連接點列表
    std::set<std::string> junction;

    任務要求: 基於DFS，找出朋友關係圖彼此不相連的每個連通成分

    for() 遍歷節點
      
      檢查cur_node是否已計算過了，計算過了的話就直接跳過
      position = ans_list.find(cur_node);
      if (position != ans_list.end()) {
        continue;
      }

      檢查cur_node是否在visited裡面      
      if (position != visited.end()) {
        junction.emplace(vertex);
        for(const auto& connection_vertex : position -> second) {
          if (junction.count(connection_vertex) == 1 || connection_vertex == root.first) {
            continue;
          } else {
            junction.emplace(connection_vertex);
          }
        }
      } else { // 代表還沒算過
        for (const auto& new_recipient : filter_list[vertex]) {
          if (junction.count(new_recipient.first) == 1 || new_recipient.first == root.first) {
            continue;
          }
          recipient.push(new_recipient.first);
          junction.emplace(new_recipient.first);
        }
      }


      visited[root.first] = junction;
      // std::cout << visited[root.first].size() << " ";
      junction.clear();
      total_id++;
    */
  }
};

class ProgramPackage {
  // 限class內部可讀寫的變數
 private:
  std::vector<DataType> dataset;  // 資料集，儲存從Bin檔讀取出來的資料
  std::string file_number;        // 檔案序號，例如pairs501.bin 的 501
  AdjacencyLists lists;  // 相鄰串列，key為發訊者學號，value為收訊者學號及其權重的map
  float threshold = 0.0;  // 有效門檻值，預設為0.0
  // 外部可讀寫的變數
 public:
  bool doneMissionOne = false;  // 確認是否執行過任務一的boolean值
  // 外部可使用的函式
 public:
  // 讀取指定Bin檔中的資料
  bool ReadBinFile() {
    doneMissionOne = false;
    std::ifstream file;
    std::string file_name, file_line;
    // 列印輸入提示及讀取檔案編號
    std::cout << "Input a file number ([0] Quit): ";
    std::cin >> file_number;
    // 輸出風格調整空行
    std::cout << "\n";
    // 如果輸入為0，跳離讀檔函式
    if (file_number == "0") {
      return false;
    }
    // 組合完整檔名
    file_name = "pairs" + file_number + ".bin";
    //  開啟檔案 // in(input) / out(output)
    file.open(file_name, std::ios::in | std::ios::binary);
    // 確認檔案是否存在，如果不存在:輸出錯誤，跳離讀檔函式，存在:繼續執行讀取檔案內容
    if (!file.is_open()) {
      // 輸出錯誤:檔案不存在
      std::cout << "### " << file_name << " does not exist!" << " ###"
                << "\n\n";
      return false;
    }
    // 讀取檔案內資料部分
    // 清除之前執行過的資料集
    dataset.clear();
    DataType data;
    // 每次從檔案中讀取一個struct的資料，存進dataset中，重複執行直到檔案中讀不到新資料
    while (file.read(reinterpret_cast<char *>(&data), sizeof(data))) {
      dataset.push_back(data);
    }
    // 關閉檔案
    file.close();
    return true;
  }
  // 寫入.adj檔
  void WriteAdjFile() {
    std::ofstream file;
    // 組合完整檔名
    std::string file_name = "pairs" + file_number + "_" + threshold + ".adj";
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << "<<< There are " << adj_list.list.size() << " IDs in total. >>>" << "\n";
    for (std::pair<std::string, std::map<std::string, float>> position : adj_list.list) {
      file << "[" << std::setw(3) << count + 1 << "] " << position.first << ": " << "\n";
      int receive_count = 1;
      for (std::pair<std::string, float> data : position.second) {
        file << "\t" << "(" << std::setw(2) << receive_count << ") " << data.first << ",   " << std::setw(4) << data.second;
        if (receive_count % 12 == 0) {
          file << "\n";
        }
        receive_count++;
      }
      file << "\n";
      count++;
    }
    file << "<<< There are " << dataset.size() << " nodes in total. >>>" << "\n";
    //  關閉檔案
    file.close();
    return;
  }
  bool CheckIsFloat(std::string& str) {
    for (int i = 0; i < str.length(); i++) {
      if (!std::isdigit(str[i]) && str[i] != '.') {
        return false;
      }
    }
    return true;
  }
  // 讀入門檻
  float GetThresholdInput() {
    std::string real_string;
    float real_number = 0.0;
    std::cout << "Input a real number in (0,1]: ";
    while (true) {
      std::cout << "\n" << "Input a real number in [0,1]: ";
      std::cin >> real_string;
      if (!CheckIsFloat(real_string)) {
        continue;
      }
      real_number = std::stof(real_string);
      if (real_number > 0 && real_number <= 1) {
        std::cout << "\n";
        return real_number;
      }
      std::cout << "\n" << "### It is NOT in (0,1] ###" << "\n";
    }
  }

  void BuildAdjacencyLists() {
    threshold = GetThresholdInput();
    
  }
};

class System {
 private:
  ProgramPackage program_package;

 public:
  void PrintUI() {
    std::cout << "**********  Graph data applications  *********" << std::endl
              << "* 1. Build a graph and connected components  *" << std::endl
              << "* 2. Find shortest paths by Dijkstra         *" << std::endl
              << "**********************************************" << std::endl;
    return;
  }
  // 讀取執行指令
  int ReadCommand() {
    std::string command;
    // 列印UI
    PrintUI();
    // 列印輸入提示及讀取執行指令
    std::cout << "Input a choice(0, 1, 2) [0: QUIT]: ";
    std::cin >> command;
    // 根據執行指令執行不同動作
    if (command == "0") {
      // 輸出風格調整空行
      std::cout << "\n";
      return 0;
    } else if (command == "1" || command == "2") {
      CallProgram(std::stoi(command));
    } else {
      // 輸出風格調整空行
      std::cout << "\n";
      std::cout << "The command does not exist!" << "\n\n";
    }
    return 4;
  }
  // 執行指定程式
  void CallProgram(int command) {
    // 執行任務一
    if (command == 1) {
      std::cout << "\n";
      // 確認讀檔是否順利，不順利則跳離
      if (!program_package.ReadBinFile()) {
        return;
      }
      // 讀檔順利會執行任務一
      program_package.BuildAdjacencyLists();
    } else if (command == 2) {  // 執行任務二
      if (program_package.doneMissionOne == false) {
        std::cout << "### There is no graph and choose 1 first. ###" << "\n\n";
        return;
      }
      program_package.ComputeConnectionCounts();
    } 
    return;
  }
};

int main() {
  System system;
  while (true) {
    if (system.ReadCommand() == 0) {
      break;
    }
  }
  return 0;
}