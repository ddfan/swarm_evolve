/// ---------------------------------------------------------------------------
/// @section LICENSE
///  
/// Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
///               All Rights Reserved
///  
/// The above copyright notice and this permission notice shall be included in 
/// all copies or substantial portions of the Software.
///  
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.
/// ---------------------------------------------------------------------------
/// @file filename.ext
/// @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
/// @author Eric Squires <eric.squires@gtri.gatech.edu>
/// @version 1.0
/// ---------------------------------------------------------------------------
/// @brief A brief description.
/// 
/// @section DESCRIPTION
/// A long description.
/// ---------------------------------------------------------------------------
#include <iostream>
#include <chrono>
#include <ctime>
#include <signal.h>
#include <string>

#include <unordered_set>
#include <cstdlib>
#include <string>
#include <ostream>
#include <cstdlib>
#include <memory>
#include <scrimmage/common/Random.h>

#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/common/Utilities.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/plugin_manager/PluginManager.h>

#include <scrimmage/entity/Entity.h>
#include <scrimmage/autonomy/Autonomy.h>
#include <scrimmage/entity/Contact.h>

#include <scrimmage/simcontrol/SimControl.h>

#include <scrimmage/network/Interface.h>
#include <scrimmage/metrics/Metrics.h>


#include "tiny_dnn/tiny_dnn.h"
#include <boost/filesystem.hpp>


// #if ENABLE_VIEWER==1
// #include <scrimmage/viewer/Viewer.h>
// #endif

#include <scrimmage/log/Log.h>

#if ENABLE_PYTHON_BINDINGS==1
#include <Python.h>
#endif

using std::cout;
using std::endl;
using namespace tiny_dnn;
using namespace tiny_dnn::layers;

namespace sc = scrimmage;

//int num_threads=0;
//sc::SimControl simcontrol[1];

// Handle kill signal
void HandleSignal(int s)
{
//     cout << endl << "Exiting gracefully" << endl;
//     for(int i=0;i<num_threads;i++)
//         simcontrol[i].force_exit();
}

std::vector<double> str2vec(std::string str)
{
    std::string temp;
    std::vector<double> vec;

    size_t i = 0, start = 0, end;

    do {
        end = str.find_first_of ( ' ', start );
        temp = str.substr( start, end );
        if ( isdigit ( temp[0] ) )
        {
            vec.push_back ( atof ( temp.c_str ( ) ) );
            ++i;
        }
        start = end + 1;
    } while ( start );

    return vec;
}

int main(int argc, char *argv[])
{
    //parse mission file
    sc::MissionParsePtr main_mp = std::make_shared<sc::MissionParse>();
    if (!main_mp->parse(argv[optind])) {
        cout << "Failed to parse file: " << argv[optind] << endl;
        return -1;
    }
    main_mp->create_log_dir();

    size_t seed = std::stoi(main_mp->params()["seed"]);
    size_t num_threads = std::stoi(main_mp->params()["num_samples_per_generation"]);
    size_t num_generations = std::stoi(main_mp->params()["num_generations"]);
    double learning_rate = std::stof(main_mp->params()["learning_rate"]);
    size_t test_every_n_generations = std::stof(main_mp->params()["test_every_n_generations"]);
    std::vector<double> param_vec = str2vec(main_mp->params()["param_vector"]);
    double sigma_ = param_vec[0];
    int n_friends_ = param_vec[1];
    int n_enemies_ = param_vec[2];
    double beta1 = param_vec[3];
    double beta2 = param_vec[4];
    double epsilon = param_vec[5];
    double weight_decay = param_vec[6];

    sc::Random random;
    random.seed(0);

    // Handle kill signals
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = HandleSignal;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);

    //ensure correct usage
    if (optind >= argc || argc < 2) {
        cout << "usage: " << argv[0] << " scenario.xml" << endl;
        return -1;
    }

    //initialize neural network
    tiny_dnn::network<tiny_dnn::sequential> policy_network;
    tiny_dnn::network<tiny_dnn::sequential> policy_network2;
    size_t num_inputs_ = 9+4+4+10*n_friends_+7*n_enemies_;
    size_t loadnn = std::stoi(main_mp->params()["start_with_loaded_nn"]);
    if(loadnn){
        policy_network.load(main_mp->params()["load_nn_path"]);
        policy_network2.load(main_mp->params()["load_nn_path2"]);
    }else{
        //number of inputs to nn are: num_self_states(9) + rel_home_base_coords(4) + rel_enemy_base_coords(4)
        // + rel_friends_states(10*n_friends_) + rel_enemy_states(7*n_enemies_)
        policy_network << fc(num_inputs_,200) << tiny_dnn::activation::tanh()
                       << fc(200,200) << tiny_dnn::activation::tanh()
                       << fc(200,50) << tiny_dnn::activation::tanh()
                       << fc(50,3) << tiny_dnn::activation::tanh();
        policy_network.weight_init(weight_init::lecun());
        policy_network.bias_init(weight_init::lecun());
        policy_network.init_weight();

        policy_network2 << fc(num_inputs_,200) << tiny_dnn::activation::tanh()
                       << fc(200,200) << tiny_dnn::activation::tanh()
                       << fc(200,50) << tiny_dnn::activation::tanh()
                       << fc(50,3) << tiny_dnn::activation::tanh();
        policy_network2.weight_init(weight_init::lecun());
        policy_network2.bias_init(weight_init::lecun());
        policy_network2.init_weight();
    }

    std::string nn_path =main_mp->log_dir() + "/init_nn.dat";
    policy_network.save(nn_path);
    std::string nn_path2 =main_mp->log_dir() + "/init_nn2.dat";
    policy_network2.save(nn_path2);

    tiny_dnn::network<tiny_dnn::sequential> zero_network;
    zero_network << fc(num_inputs_,200) << tiny_dnn::activation::tanh()
                 << fc(200,200) << tiny_dnn::activation::tanh()
                 << fc(200,50) << tiny_dnn::activation::tanh()
                 << fc(50,3) << tiny_dnn::activation::tanh();
    sc::AdamOptimizer<vec_t> adamoptimizer;
    sc::AdamOptimizer<vec_t> adamoptimizer2;
    adamoptimizer.setparams(beta1,beta2,epsilon);
    adamoptimizer2.setparams(beta1,beta2,epsilon);


    //start main loop
    std::vector<double> scores(num_threads,0);
    std::ofstream score_history_file(main_mp->log_dir() + "/scores.txt");
    std::ofstream test_score_history_file(main_mp->log_dir() + "/test_scores.txt");

    std::vector<double> scores2(num_threads,0);
    std::ofstream score_history_file2(main_mp->log_dir() + "/scores2.txt");
    std::ofstream test_score_history_file2(main_mp->log_dir() + "/test_scores2.txt");

    bool testing = false;
    size_t n=0;
    while(n<num_generations)
    {
        if(n % test_every_n_generations == 0){
            if(!testing)
                testing=true;
            else
                testing=false;
        }

        if(testing){
            testing=true;
            param_vec[0]=0.0;
            std::cout<<"Testing... ";
        }else{
            param_vec[0]=sigma_;
            std::cout<<"Iteration #" << n+1 << ", ";
        }

        random.seed(seed+n+1);
        std::vector<sc::SimControl> simcontrol(num_threads);
        std::vector<sc::MissionParsePtr> mp(num_threads);

#if ENABLE_PYTHON_BINDINGS==1
    Py_Initialize();
#endif

        for(size_t i=0;i<num_threads;i++)
        {
            // Parse mission config file:
            mp[i]=std::make_shared<sc::MissionParse>();
            mp[i]->set_task_number(i);
            mp[i]->set_job_number(n);
            if (!mp[i]->parse(argv[optind])) {
                cout << "Failed to parse file: " << argv[optind] << endl;
                return -1;
            }

            //use our custom log directory structure
            if(testing)
                mp[i]->set_log_dir(main_mp->log_dir() + "/test/gen" + std::to_string(n) + "/job" + std::to_string(i));
            else
                mp[i]->set_log_dir(main_mp->log_dir() + "/gen" + std::to_string(n) + "/job" + std::to_string(i));
            mp[i]->create_log_dir(false);

            //set unique seed for each thread
            mp[i]->params()["seed"] = std::to_string(random.rng_uniform_int(100,99999999));

            // Setup Logger
            std::shared_ptr<sc::Log> log(new sc::Log());
            log->set_enable_log(false);
            log->init(mp[i]->log_dir(), sc::Log::NONE);
            simcontrol[i].set_log(log);

            //dunno what this does, remove it?
            sc::InterfacePtr to_gui_interface = std::make_shared<sc::Interface>();
            sc::InterfacePtr from_gui_interface = std::make_shared<sc::Interface>();
            to_gui_interface->set_log(log);
            from_gui_interface->set_log(log);

            simcontrol[i].set_incoming_interface(from_gui_interface);
            simcontrol[i].set_outgoing_interface(to_gui_interface);

            simcontrol[i].set_mission_parse(mp[i]);
            simcontrol[i].set_parameter_vector(param_vec);
            simcontrol[i].set_nn_path(nn_path);
            simcontrol[i].set_nn_path2(nn_path2);


            // Split off SimControl in it's own thread
            if (!simcontrol[i].init()) {
                cout << "SimControl init() failed." << endl;
                return -1;
            }
            simcontrol[i].display_progress(false);
            simcontrol[i].start();
        }

        // Make sure SimControl joins properly before program ends
        for(size_t i=0;i<num_threads;i++){
            simcontrol[i].join();
        }

#if ENABLE_PYTHON_BINDINGS==1
    Py_Finalize();
#endif

        // collect scores, do logging, add up score-weighted nn weights
        double totalscore=0.0;
        int num_notnan_scores=0;
        double totalscore2=0.0;
        int num_notnan_scores2=0;
        for(size_t i=0;i<num_threads;i++)
        {
            // calculate scores
            std::map<int, double> team_scores;
            std::map<int, std::map<std::string, double>> team_metrics;
            std::list<std::string> headers;

            // Loop through each of the metrics plugins.
            for (sc::MetricsPtr metrics : simcontrol[i].metrics()) {
//                cout << sc::generate_chars("=", 80) << endl;
//                cout << metrics->name() << endl;
//                cout << sc::generate_chars("=", 80) << endl;
                metrics->calc_team_scores();
//                metrics->print_team_summaries();

                // Add all elements from individual metrics plugin to overall
                // metrics data structure
                for (auto const &team_str_double : metrics->team_metrics()) {
                    team_metrics[team_str_double.first].insert(team_str_double.second.begin(),
                                                               team_str_double.second.end());
                }

                // Calculate aggregated team scores:
                for (auto const &team_score : metrics->team_scores()) {
                    if (team_scores.count(team_score.first) == 0) {
                        team_scores[team_score.first] = 0;
                    }
                    team_scores[team_score.first] += team_score.second;
                }

                // Create list of all csv headers
                headers.insert(headers.end(), metrics->headers().begin(),
                               metrics->headers().end());
            }

            // Create headers string
            std::string csv_str = "team_id,score";
            for (std::string header : headers) {
                csv_str += "," + header;
            }
            csv_str += "\n";

            // Loop over each team and generate csv output
            for (auto const &team_str_double : team_metrics) {

                // Each line starts with team_id,score
                csv_str += std::to_string(team_str_double.first);
                csv_str += "," + std::to_string(team_scores[team_str_double.first]);

                // Loop over all possible headers, if the header doesn't exist for
                // a specific team, default the value for that header to zero.
                for (std::string header : headers) {
                    csv_str += ",";

                    auto it = team_str_double.second.find(header);
                    if (it != team_str_double.second.end()) {
                        csv_str += std::to_string(it->second);
                    } else {
                        csv_str += std::to_string((double)0);
                    }
                }
                csv_str += "\n";
            }


//            // Print Overall Scores
//            cout << sc::generate_chars("=", 80) << endl;
//            cout << "Overall Scores" << endl;
//            cout << sc::generate_chars("=", 80) << endl;
//            for (auto const &team_score : team_scores) {
//                cout << "Team ID: " << team_score.first << endl;
//                cout << "Score: " << team_score.second << endl;
//                cout << sc::generate_chars("-", 80) << endl;
//            }

            // Write CSV string to file
            std::string out_file = mp[i]->log_dir() + "/summary.csv";
            std::ofstream summary_file(out_file);
            if (!summary_file.is_open()) {
                std::cout << "could not open " << out_file
                          << " for writing metrics" << std::endl;
                return -1;
            }
            summary_file << csv_str << std::flush;
            summary_file.close();

            //collect scores
            scores[i]=team_scores[1];
            scores2[i]=team_scores[2];

            if (!std::isnan(scores[i])){
                totalscore+=scores[i];
                num_notnan_scores++;
            }else{
                scores[i]=-1e100;
            }

            if (!std::isnan(scores2[i])){
                totalscore2+=scores2[i];
                num_notnan_scores2++;
            }else{
                scores2[i]=-1e100;
            }
        }

        //log time spent and scores
        if(testing){
            std::ofstream runtime_file(main_mp->log_dir() + "/test/gen" + std::to_string(n) + "/info.txt");
            runtime_file << "Avg scores team 1: "<<totalscore/(double)num_notnan_scores<<"   Avg scores team 2: "<<totalscore2/(double)num_notnan_scores2<<std::endl;
            runtime_file << "team 1 scores: " << std::endl;
            for (size_t i=0;i<num_threads;i++)
                runtime_file << scores[i] << " ";
            runtime_file << std::endl << "team 2 scores: " << std::endl;
            for (size_t i=0;i<num_threads;i++)
                runtime_file << scores2[i] << " ";
            runtime_file.close();

            for (size_t i=0;i<num_threads;i++)
                test_score_history_file << scores[i] << " ";
            test_score_history_file << std::endl;
            for (size_t i=0;i<num_threads;i++)
                test_score_history_file2 << scores2[i] << " ";
            test_score_history_file2 << std::endl;

            std::cout << "Avg scores team 1: "<<totalscore/(double)num_notnan_scores<<"   Avg scores team 2: "<<totalscore2/(double)num_notnan_scores2<<std::endl;
            continue;
        }

        //log time spent and scores
        std::ofstream runtime_file(main_mp->log_dir() + "/gen" + std::to_string(n) + "/info.txt");
        runtime_file << "Avg scores team 1: "<<totalscore/(double)num_notnan_scores<<"   Avg scores team 2: "<<totalscore2/(double)num_notnan_scores2<<std::endl;
        runtime_file << "team 1 scores: " << std::endl;
        for (size_t i=0;i<num_threads;i++)
            runtime_file << scores[i] << " ";
        runtime_file << std::endl << "team 2 scores: " << std::endl;
        for (size_t i=0;i<num_threads;i++)
            runtime_file << scores2[i] << " ";
        runtime_file.close();

        for (size_t i=0;i<num_threads;i++)
            score_history_file << scores[i] << " ";
        score_history_file << std::endl;
        for (size_t i=0;i<num_threads;i++)
            score_history_file2 << scores2[i] << " ";
        score_history_file2 << std::endl;

        std::cout << "Avg scores team 1: "<<totalscore/(double)num_notnan_scores<<"   Avg scores team 2: "<<totalscore2/(double)num_notnan_scores2<<std::endl;

        //normalize scores with rank transformation
        std::vector<double> rank(scores.size());
        std::vector<double> rank_scores(scores.size());
        std::size_t m(0);
        std::generate(std::begin(rank), std::end(rank), [&]{ return m++; });
        std::sort(  std::begin(rank), std::end(rank), [&](double i1, double i2) { return scores[i1] < scores[i2]; } );

        //we want identical scores to receive the same rank
        if(num_threads>1){
            size_t num_similar=0;
            double last_value=0.0;
            rank_scores[rank[0]]=0.0;
            last_value=scores[rank[0]];
            for(size_t i=1;i<num_threads;i++){
                if(last_value == scores[rank[i]])
                    num_similar++;
                rank_scores[rank[i]]=(double) (i-num_similar);
                last_value=scores[rank[i]];
            }
            if(num_similar==m-1)
                for(size_t i=0;i<num_threads;i++)
                    rank_scores[i]=1.0;
            for(size_t i=0;i<num_threads;i++)
                rank_scores[i]=(double)(rank_scores[i]+1)/(double)(m-num_similar) - 0.5;

        }else{
            rank_scores[0]=1.0;
        }

        //update zero neural network with score-weighted weights, to basically keep track of the total gradient
        zero_network.weight_init(weight_init::constant(0.0));
        zero_network.bias_init(weight_init::constant(0.0));
        zero_network.init_weight();
        random.seed(seed+n+1);
        for(size_t i=0;i<num_threads;i++){
            zero_network.perturb_weights(random.rng_uniform_int(100,99999999), 1.0/(double)num_threads/sigma_*rank_scores[i]);
        }

        //apply optimizer update to policy_network
        vec_t grad = zero_network.get_weights();
        vec_t update = adamoptimizer.step(grad);
        vec_t theta = policy_network.get_weights();
        for(size_t i=0;i<theta.size();i++)
            theta[i]=weight_decay*theta[i] + learning_rate*update[i];

        policy_network.set_weights(&theta);

        //save nn
        nn_path = main_mp->log_dir() + "/gen" + std::to_string(n) + "/nn.dat";
        policy_network.save(nn_path);



        //normalize scores with rank transformation
        m=0;
        std::generate(std::begin(rank), std::end(rank), [&]{ return m++; });
        std::sort(  std::begin(rank), std::end(rank), [&](double i1, double i2) { return scores2[i1] < scores2[i2]; } );

        //we want identical scores to receive the same rank
        if(num_threads>1){
            size_t num_similar=0;
            double last_value=0.0;
            rank_scores[rank[0]]=0.0;
            last_value=scores2[rank[0]];
            for(size_t i=1;i<num_threads;i++){
                if(last_value == scores2[rank[i]])
                    num_similar++;
                rank_scores[rank[i]]=(double) (i-num_similar);
                last_value=scores2[rank[i]];
            }
            if(num_similar==m-1)
                for(size_t i=0;i<num_threads;i++)
                    rank_scores[i]=1.0;
            for(size_t i=0;i<num_threads;i++)
                rank_scores[i]=(double)(rank_scores[i]+1)/(double)(m-num_similar) - 0.5;

        }else{
            rank_scores[0]=1.0;
        }

        //update zero neural network with score-weighted weights, to basically keep track of the total gradient
        zero_network.weight_init(weight_init::constant(0.0));
        zero_network.bias_init(weight_init::constant(0.0));
        zero_network.init_weight();
        random.seed(seed+n+1);
        for(size_t i=0;i<num_threads;i++){
            zero_network.perturb_weights(random.rng_uniform_int(100,99999999)+1, 1.0/(double)num_threads/sigma_*rank_scores[i]);
        }

        //apply optimizer update to policy_network
        grad = zero_network.get_weights();
        update = adamoptimizer2.step(grad);
        theta = policy_network2.get_weights();
        for(size_t i=0;i<theta.size();i++)
            theta[i]=weight_decay*theta[i] + learning_rate*update[i];

        policy_network2.set_weights(&theta);

        //save nn
        nn_path2 = main_mp->log_dir() + "/gen" + std::to_string(n) + "/nn2.dat";
        policy_network2.save(nn_path2);

        n++;
    }

    // Close the log file
    // log->close_log();
    score_history_file.close();
    test_score_history_file.close();
    score_history_file2.close();
    test_score_history_file2.close();

    cout << "Simulation Complete" << endl;
    return 0;
}
