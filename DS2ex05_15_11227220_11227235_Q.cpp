// 資訊二乙 11227220張季平、11227235吳君友
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <queue>
#include <utility>  // std::pair
#include <map>
#include <unordered_map>
#include <format>
#include <set>
#include <print>

constexpr int infinity = 2;

// Bin檔中的資料型態
struct DataType {
  std::array<char, 12> send_id{};     // 發訊者學號
  std::array<char, 12> receive_id{};  // 收訊者學號
  float weight;                       // 量化權重
};

struct DistanceType {
  std::string id;
  float distance;
};

struct ShortestPathType {
  std::string startid;
  std::vector<DistanceType> distance;
  void InputDistance(const std::map<std::string, float>& dist) {
    distance.clear();
    DistanceType temp;
    bool done = false;
    for (const auto& [id, dis] : dist) {
      if (id == startid) {
        continue;
      }
      temp.id = id;
      temp.distance = dis;
      done = false;
      for(int i = 0; i < distance.size(); i++) {
        if (temp.distance < distance[i].distance) {
          distance.insert(distance.begin() + i, temp);
          done = true;
          break;
        }
      }
      if (!done) {
        distance.emplace_back(temp);
      }
    }
  }
};

// 定義任務一外層集合的排列方式: 先依集合大小降序，再依字典序降序
struct SetSizeThenLexDescCompare {
  bool operator()(const std::set<std::string>& a,
                  const std::set<std::string>& b) const {
      if (a.size() != b.size())
          return a.size() > b.size();  
      return std::lexicographical_compare(b.begin(), b.end(), a.begin(), a.end());
  }
};

class AdjacencyLists {
 public:
  std::map<std::string, std::map<std::string, float>> adjacency_list;  // adjcency_list 相鄰串列
  std::set<std::set<std::string>, SetSizeThenLexDescCompare> connection_list;  // ans_list (vector of string set)
 public:
  // 清除之前執行過的資料紀錄
  void ClearRecords() {
    adjacency_list.clear();
    connection_list.clear();
    return;
  }
  void BuildAdjacencyLists(const std::vector<DataType>& dataset, const float& threshold) {
    for (const auto& data : dataset) {
      std::string send_id(data.send_id.data());
      std::string receive_id(data.receive_id.data());
      if (data.weight <= threshold) {
        adjacency_list[send_id][receive_id] = data.weight;
        adjacency_list[receive_id][send_id] = data.weight;
      }
    }
  }

  int CalculateConnectionNumber() {
    std::stack<std::string> recipient; // 接收者堆疊

    std::set<std::string> visited; // 記錄任務一所有走過的節點
    std::set<std::string> current_group; // 記錄當前連通走過了哪裡

    std::string vertex;
    int total_id = 0;

    for (const auto& root : adjacency_list) {
      // 如果此節點已被收錄在任一連通裡，直接跳過找下一個節點
      if (visited.count(root.first) == 1) {
        continue; 
      }

      recipient.push(root.first);
      visited.emplace(root.first);

      while (!recipient.empty()) {
        vertex = recipient.top();
        recipient.pop();
        current_group.emplace(vertex);

        for (const auto& new_recipient : adjacency_list[vertex]) {
          if (!visited.count(new_recipient.first)) {
            recipient.push(new_recipient.first);
            visited.emplace(new_recipient.first);
          }
        }
      }

      // 記錄完當前連通後，加到答案串列裡
      connection_list.emplace(current_group);
      current_group.clear();
      total_id++;
    }

    return total_id;
  }

  // 實作
  void Dijkstra(const std::string& start_id, std::map<std::string, float>& distance) {
    std::queue<std::string> path;
    std::map<std::string, std::set<std::string>> visited;
    std::map<std::string, float> neighbor;
    std::string vertex;
    std::vector<DistanceType> sorted_neighbor;
    bool done = false;
    // 起點開始
    path.emplace(start_id);
    while (!path.empty()) {
      vertex = path.front();
      path.pop();
      // 從adjacency_list中獲取指定名字的相鄰串列
      neighbor = adjacency_list[vertex];
      
      // 為了由小到大輸入，重新排序鄰居
      for (const auto& [id, weight] : neighbor) {
        DistanceType temp = {id, weight};
        done = false;
        for (int i = 0; i < sorted_neighbor.size(); i++) {
          if (temp.distance < sorted_neighbor[i].distance) {
            sorted_neighbor.insert(sorted_neighbor.begin() + i, temp);
            done = true;
            break;
          }
        }
        if (!done) {
          sorted_neighbor.emplace_back(temp);
        }
      }
      // 導入queue中，更新distance
      for (const auto& data : sorted_neighbor) {
        if (visited[data.id].empty()) {
          path.emplace(data.id);
        }
        if (distance[vertex] + data.distance < distance[data.id]) {
          distance[data.id] = distance[vertex] + data.distance;
        }
        visited[data.id].emplace(vertex);
      }
      sorted_neighbor.clear();
    }
  }
  float Prim(const std::set<std::string>& connected_component) {
    std::map<std::string, float> adjacency_vertex = adjacency_list[connected_component.begin()->data()];
    std::priority_queue<std::pair<float, std::string>, std::vector<std::pair<float, std::string>>, std::greater<std::pair<float, std::string>>> priority_queue;
    std::set<std::string> visited;
    std::string vertex;
    float weight;
    float dist;
    // 將第一個點的所有連接點資訊加入到優先佇列中
    for (const auto& [id, dist] : adjacency_vertex) {
      if (visited.count(id) == 0) {
        priority_queue.emplace(dist, id);
      }  else {
        continue;
      }
    }
    // 第一個點列為visited
    visited.emplace(connected_component.begin()->data());
    for (int count = 0; count < connected_component.size() - 1; count++) {
      // 取得下一個未拜訪過的最小距離連接點
      while (true) {
        vertex = priority_queue.top().second;
        weight = priority_queue.top().first;
        priority_queue.pop();
        if (visited.count(vertex) == 0) {
          break;
        }
      }
      // 更新目前總距離
      dist += weight;
      // 將點設為visited
      visited.emplace(vertex);
      // 取得點的相鄰串列
      adjacency_vertex = adjacency_list[vertex];
      // 此點的所有連接點資訊加入到優先佇列中
      for (const auto& [id, dist] : adjacency_vertex) {
        if (visited.count(id) == 0) {
          priority_queue.emplace(dist, id);
        }
      }
    }
    return dist;
  }
  // 計算最短路徑
  void FindShortestPath(const std::string& start_id, ShortestPathType& result) {
    // 第一步 從connection_list中 找到 start_id 在的那個 Connected Component，copy下來
    std::set<std::string> connected_component;
    for (const auto& set : connection_list) {
      if (set.count(start_id) == 1) {
        connected_component = set;
        break;
      }
    }
    // 第二步 建立一個map，存放點對應最短路徑
    std::map<std::string, float> distance;
    for (const auto& id : connected_component) {
      if (start_id == id) { 
        distance[id] = 0;
      } else {
        distance[id] = infinity;
      }
    }
    // 第三步 呼叫Dijkstra算最短路徑
    Dijkstra(start_id, distance);
    // 最後 新增到shortest_path
    result.startid = start_id;
    result.InputDistance(distance);
    return;
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
 // **********讀寫檔案程式碼區域**********
  // 讀取指定Bin檔中的資料
  void ReadBinFile() {
    doneMissionOne = false;
    std::ifstream file;
    std::string file_name, file_line;
    // 清除之前執行過的資料集
    dataset.clear();
    // 列印輸入提示及讀取檔案編號
    std::print("Input a file number ([0] Quit): ");
    std::cin >> file_number;
    // 如果輸入為0，跳離讀檔函式
    if (file_number == "0") {
      return;
    }
    // 組合完整檔名
    file_name = std::format("pairs{}.bin", file_number);
    //  開啟檔案 // in(input) / out(output)
    file.open(file_name, std::ios::in | std::ios::binary);
    // 確認檔案是否存在，如果不存在:輸出錯誤，跳離讀檔函式，存在:繼續執行讀取檔案內容
    if (!file.is_open()) {
      // 輸出錯誤:檔案不存在
      std::println("\n### {} does not exist! ###", file_name);
      return;
    }
    // 讀取檔案內資料部分
    DataType data;
    // 每次從檔案中讀取一個struct的資料，存進dataset中，重複執行直到檔案中讀不到新資料
    while (file.read(reinterpret_cast<char *>(&data), sizeof(data))) {
      dataset.push_back(data);
    }
    // 關閉檔案
    file.close();
    return;
  }
  // 寫入.adj檔
  int WriteAdjFile() {
    std::ofstream file;
    std::string file_name;
    int total_count = 0;
    // 組合完整檔名
    if (threshold == 1) {
      file_name = std::format("pairs{}_{}..adj", file_number, threshold);
    } else {
      file_name = std::format("pairs{}_{}.adj", file_number, threshold);
    }
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << std::format("<<< There are {} IDs in total. >>>\n", lists.adjacency_list.size());
    for (const auto& position : lists.adjacency_list) {
      file << std::format("[{:>3}] {}: \n", count + 1, position.first);
      int receive_count = 1;
      for (const auto& data : position.second) {
        file << std::format("\t({:>2}) {}, {:>6}", receive_count, data.first, data.second);
        if (receive_count % 12 == 0) {
          file << "\n";
        }
        receive_count++;
      }
      total_count += receive_count - 1;
      file << "\n";
      count++;
    }
    file << std::format("<<< There are {} nodes in adjacency lists. >>>\n", total_count);
    //  關閉檔案
    file.close();
    return total_count;
  }
  // 寫入.cc檔
  void WriteCcFile() {
    std::ofstream file;
    std::string file_name;
    // 組合完整檔名
    if (threshold == 1) {
      file_name = std::format("pairs{}_{}..cc", file_number, threshold);
    } else {
      file_name = std::format("pairs{}_{}.cc", file_number, threshold);
    }
    int count = 0;
    //  開啟檔案
    file.open(file_name);
    file << std::format("<<< There are {} connected components in total. >>>\n", lists.connection_list.size());
    for (const auto& set : lists.connection_list) {
      file << std::format("{{{:>2}}} Connected Component: size = {}\n", count + 1, set.size());
      int receive_count = 1;
      for (const auto& data : set) {
        file << std::format(" \t({:>3}) {}", receive_count, data);
        if (receive_count % 8 == 0) {
          file << "\n";
        }
        receive_count++;
      }
      file << "\n";
      count++;
    }
    //  關閉檔案
    file.close();
    return;
  }

  // 寫入.ds檔
  void WriteDsFile(const ShortestPathType& result) {
    std::ofstream file;
    std::string file_name;
    // 組合完整檔名
    if (threshold == 1) {
      file_name = std::format("pairs{}_{}..ds", file_number, threshold);
    } else {
      file_name = std::format("pairs{}_{}.ds", file_number, threshold);
    }
    int count = 1;
    //  開啟檔案
    file.open(file_name, std::ios::app);
    file << std::format("\norigin: {}\n", result.startid);
    for (const auto& [id, dist]: result.distance) {
      if (int(dist * 100) % 10 == 0) {
        file << std::format("({:>2}) \t{}, {:.1f}\t", count, id, dist);
      } else {
        file << std::format("({:>2}) \t{}, {:.2f}\t", count, id, dist);
      }
      if (count % 8 == 0) {
        file << "\n";
      }
      count++;
    }
    file << "\n";
    //  關閉檔案
    file.close();
    return;
  }

 // **********讀寫檔案程式碼區域結束**********

 // **********任務零程式碼區域**********
  // 檢查是否是float
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
    while (true) {
      std::print("\nInput a real number in (0,1]: ");
      std::cin >> real_string;
      if (!CheckIsFloat(real_string)) {
        continue;
      }
      real_number = std::stof(real_string);
      if (real_number > 0 && real_number <= 1) {
        std::print("\n");
        return real_number;
      }
      std::println("\n### It is NOT in (0,1] ###");
    }
  }
  // 任務零：建立相鄰串列
  bool BuildAdjacencyLists() {
    // 清除之前執行過的資料集
    lists.ClearRecords();
    threshold = GetThresholdInput();
    ReadBinFile();
    if (dataset.empty()) {
      std::println("### There is no graph and try it again. ###\n");
      return false;
    }
    lists.BuildAdjacencyLists(dataset, threshold);
    if (lists.adjacency_list.empty()) {
      std::println("### There is no graph and try it again. ###\n");
      return false;
    }
    // 寫入.adj檔
    int total_id_in_adjacencylist = WriteAdjFile();
    std::println("\n<<< There are {} IDs in total. >>>\n", lists.adjacency_list.size());
    std::println("<<< There are {} nodes in adjacency lists. >>>\n", total_id_in_adjacencylist);
    return true;
  }

 // **********任務零程式碼區域結束**********

 // **********任務一程式碼區域**********
  // 任務一：尋找連通成分
  void FindConnectedComponents() {
    if (!BuildAdjacencyLists()) {
      return;
    }
    lists.CalculateConnectionNumber();
    WriteCcFile();
    std::println("<<< There are {} connected components in total. >>>", lists.connection_list.size());
    int count = 1;
    for (const auto& set : lists.connection_list) {
      std::println("{{{:>2}}} Connected Component: size = {}", count, set.size());
      count++;
    }
    std::print("\n");
    doneMissionOne = true;  // 任務一完成
    return;
  }
 // **********任務一程式碼區域結束**********

 // **********任務二程式碼區域**********
  // 任務二：計算最短路徑
  void CalculateShortestPaths() {
    std::string start_id;
    ShortestPathType result;
    int count = 0;
    while (true) {
      // 列印學號清單
      count = 1;
      for (const auto& position : lists.adjacency_list) {
        std::print("    {:>8}", position.first);
        if (count % 8 == 0) {
          std::print("\n");
        }
        count++;
      }
      std::print("\n");
      std::print("Input a student ID [0: exit] ");
      std::cin >> start_id;
      std::print("\n");
      if (start_id  == "0") {
        break;
      }
      if (lists.adjacency_list.count(start_id) == 0) {
        std::println("### the student id does not exist! ###\n");
        continue;
      }
      std::print("\n");
      lists.FindShortestPath(start_id, result);
      WriteDsFile(result);
    }
  }
 // **********任務二程式碼區域結束**********
 // **********任務三程式碼區域**********
  // 找到最小生成樹，使用Prim演算法
  void FindMST() {
    int index = 1;
    // it是指向connection_list的位置
    for (auto it : lists.connection_list) {
      std::println("The MST cost of connected component {{{:>2}}} = {:.2f}", index, lists.Prim(it));
      index++;
    }
    std::print("\n");
    return;
  }
 // **********任務三程式碼區域結束**********
};

class System {
 private:
  ProgramPackage program_package;

 public:
  void PrintUI() {
    std::cout << "**********  Graph data applications  *********" << std::endl
              << "* 1. Build a graph and connected components  *" << std::endl
              << "* 2. Find shortest paths by Dijkstra         *" << std::endl
              << "* 3. Generate minimum spanning tree(s)       *" << std::endl
              << "**********************************************" << std::endl;
    return;
  }
  // 讀取執行指令
  int ReadCommand() {
    std::string command;
    // 列印UI
    PrintUI();
    // 列印輸入提示及讀取執行指令
    std::print("Input a choice(0, 1, 2, 3) [0: QUIT]: ");
    std::cin >> command;
    // 根據執行指令執行不同動作
    if (command == "0") {
      // 輸出風格調整空行
      std::print("\n");
      return 0;
    } else if (command == "1" || command == "2" || command == "3") {
      CallProgram(std::stoi(command));
    } else {
      // 輸出風格調整空行
      std::print("\n");
      std::println("The command does not exist!\n");
    }
    return 4;
  }
  // 執行指定程式
  void CallProgram(int command) {
    // 執行任務一
    if (command == 1) {
      // 執行任務一
      program_package.FindConnectedComponents();
    } else if (command == 2) {  // 執行任務二
      if (program_package.doneMissionOne == false) {
        std::println("### There is no graph and choose 1 first. ###\n");
        return;
      }
      std::print("\n");
      program_package.CalculateShortestPaths();

    } else if (command == 3) {  // 執行任務三
      if (program_package.doneMissionOne == false) {
        std::println("### There is no graph and choose 1 first. ###\n");
        return;
      }
      program_package.FindMST();
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