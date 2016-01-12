#ifndef QUICLI_HPP_
#define QUICLI_HPP_

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <cassert>

namespace quicli
{
    using Occurance = std::vector<std::string>;
    using ValueMap = std::map<std::string, std::vector<Occurance>>;

    std::vector<std::string> convert(int argc, char** argv)
    {
        auto list = std::vector<std::string>(argv + 1, argv + argc);
        for(auto iter = list.begin(); iter != list.end(); ++iter) {
            auto pos = std::find(iter->begin(), iter->end(), '=');
            if(pos == iter->end()) continue;
            auto index = pos - iter->begin();
            auto left  = iter->substr(0, index);
            auto right = iter->substr(index + 1);
            list.insert(iter, left);
            *iter = right;
        }
        return list;
    }

    class Argument
    {
        protected:
            std::vector<std::string> _names;
            int _priority;
            std::tuple<bool, std::string> _default;
            bool _mandatory;

        public:
            Argument(std::initializer_list<std::string> names);
            Argument(const std::string& name);

            Argument& priority(int val);
            Argument& default_value(const std::string& val);
            Argument& mandatory(bool val);

            const std::string& first_name() const;
            bool has_name(const std::string& name) const;
            int priority() const;
            bool mandatory() const;

            std::tuple<bool, std::size_t> matches(const std::string& str) const;
            virtual void extract(const std::tuple<bool, std::size_t>& match,
                                 std::vector<std::string>& args, ValueMap& vm) const = 0;
    };

    class Flag : public Argument
    {
        public:
            using Argument::Argument;

            virtual void extract(const std::tuple<bool, std::size_t>& match,
                                 std::vector<std::string>& args, ValueMap& vm) const override;
    };

    class Parameter : public Argument
    {
        private:
            std::size_t _num_vals;
            bool _multi;

        public:
            Parameter(std::initializer_list<std::string> names);
            Parameter(const std::string& name);

            Parameter& num_values(std::size_t val)
            {
                _num_vals = val;
                return *this;
            }
            Parameter& multi(bool val)
            {
                _multi = val;
                return *this;
            }

            virtual void extract(const std::tuple<bool, std::size_t>& match,
                                 std::vector<std::string>& args, ValueMap& vm) const override;
    };

    class Prompt : public Parameter
    {
        protected:
            std::function<Occurance(void)> _prompt;

        public:
            using Parameter::Parameter;

            Prompt(std::initializer_list<std::string> names, std::function<Occurance(void)> f);
            
            virtual void extract(const std::tuple<bool, std::size_t>& match,
                                 std::vector<std::string>& args, ValueMap& vm) const override;
    };

    class CLI
    {
        protected:
            std::string _name;
            std::vector<std::unique_ptr<Argument>> _args;

        public:
            CLI(const std::string& name);

            template<typename T>
            T& add(const T& arg);

            template<typename T>
            T& get(const std::string& name);

            template<typename T>
            const T& get(const std::string& name) const;

            void parse(std::vector<std::string>& args, ValueMap& vm) const;
            void validate(const ValueMap& vm) const;
            virtual std::string help() const;
    };

    
    
    
    
    
    /////////////////////////
    // class Argument
    /////////////////////////
    Argument::Argument(std::initializer_list<std::string> names) : _names(names) {}
    Argument::Argument(const std::string& name) : _names{name} {}

    Argument& Argument::priority(int val)
    {
        _priority = val;
        return *this;
    }
    Argument& Argument::default_value(const std::string& val)
    {
        std::get<1>(_default) = val;
        return *this;
    }
    Argument& Argument::mandatory(bool val)
    {
        _mandatory = val;
        return *this;
    }

    const std::string& Argument::first_name() const { return _names.front(); }
    bool Argument::has_name(const std::string& name) const
    {
        return std::find(_names.begin(), _names.end(), name) != _names.end();
    }
    int Argument::priority() const { return _priority; }
    bool Argument::mandatory() const { return _mandatory; }

    std::tuple<bool, std::size_t> Argument::matches(const std::string& str) const
    {
        auto iter = std::find(_names.begin(), _names.end(), str);
        if(iter == _names.end()) return std::make_tuple(false, 0);
        return std::make_tuple(true, iter - _names.begin());
    }
    
    
    
    /////////////////////////
    // class Flag
    /////////////////////////
    void Flag::extract(const std::tuple<bool, std::size_t>& match, std::vector<std::string>& args,
                         ValueMap& vm) const
    {
        static auto count = 0ul;
        if(!std::get<0>(match)) return;
        args.erase(args.begin());
        ++count;
        auto pos = vm.find(_names.front());
        if(pos == vm.end())
            vm.insert(std::make_pair(_names.front(), std::vector<Occurance>{{"init"}}));
        vm[_names.front()][0][0] = std::to_string(count);
    }
    
    
    
    /////////////////////////
    // class Parameter
    /////////////////////////
    Parameter::Parameter(std::initializer_list<std::string> names)
        : Argument(names), _num_vals(1), _multi(false)
    {
    }

    Parameter::Parameter(const std::string& name)
        : Argument(name), _num_vals(1), _multi(false)
    {
    }

    void Parameter::extract(const std::tuple<bool, std::size_t>& match, std::vector<std::string>& args,
                 ValueMap& vm) const
    {
        if(!std::get<0>(match)) return;
        args.erase(args.begin());
        if(_num_vals > args.size())
            throw std::runtime_error("Argument \"" + first_name() + "\" requires "
                                     + std::to_string(_num_vals) + " values, but only got "
                                     + std::to_string(args.size()));
        auto pos = vm.find(_names.front());
        if(pos == vm.end()) vm.insert(std::make_pair(_names.front(), std::vector<Occurance>()));
        vm[_names.front()].emplace_back(Occurance(args.begin(), args.begin() + _num_vals));
        args.erase(args.begin(), args.begin() + _num_vals);
    }
    
   

    /////////////////////////
    // class Prompt
    /////////////////////////
    Prompt::Prompt(std::initializer_list<std::string> names, std::function<Occurance(void)> f)
        : Parameter(names), _prompt(f)
    {
    }

    void Prompt::extract(const std::tuple<bool, std::size_t>& match, std::vector<std::string>& args,
                         ValueMap& vm) const
    {
        if(!std::get<0>(match)) return;
        args.erase(args.begin());
        auto occ = _prompt();
        auto pos = vm.find(_names.front());
        if(pos == vm.end()) vm.insert(std::make_pair(_names.front(), std::vector<Occurance>()));
        vm[_names.front()].push_back(occ);
    }



    /////////////////////////
    // class CLI
    /////////////////////////
    CLI::CLI(const std::string& name) : _name(name) {}

    template <typename T>
    T& CLI::add(const T& arg)
    {
        auto pos = std::find_if(_args.begin(), _args.end(), [&arg](std::unique_ptr<Argument>& a) {
            return a->priority() <= arg.priority();
        });
        T* tmp = new T(arg);
        _args.insert(pos, std::unique_ptr<T>(tmp));
        return *tmp;
    }

    template <typename T>
    T& CLI::get(const std::string& name)
    {
        auto pos =
            std::find_if(_args.begin(), _args.end(),
                         [&name](std::unique_ptr<Argument>& arg) { return arg->has_name(name); });
        if(pos == _args.end())
            throw std::runtime_error("No argument with name \"" + name + "\" in CLI \"" + _name
                                     + "\"");
        return *dynamic_cast<T*>(pos->get());
    }

    template<typename T>
    const T& CLI::get(const std::string& name) const
    {
        auto pos =
            std::find_if(_args.begin(), _args.end(), [&name](const std::unique_ptr<Argument>& arg) {
                return arg->has_name(name);
            });
        if(pos == _args.end())
            throw std::runtime_error("No argument with name \"" + name + "\" in CLI \"" + _name
                                     + "\"");
        return *dynamic_cast<const T*>(pos->get());
    }

    void CLI::parse(std::vector<std::string>& args, ValueMap& vm) const
    {
        while(!args.empty()) {
            bool matched = false;
            for(auto& arg : _args) {
                auto match = arg->matches(args.front());
                if(!std::get<0>(match)) continue;
                arg->extract(match, args, vm);
                matched = true;
                break;
            }
            if(!matched) throw std::runtime_error("Argument \"" + args.front() + "\" is unmatched");
        }
    }

    void CLI::validate(const ValueMap& vm) const
    {
        std::string msg = "";
        for(auto& arg : _args) {
            if(!arg->mandatory()) continue;
            auto pos = vm.find(arg->first_name());
            if(pos == vm.end()) msg += "Mandatory argument \"" + arg->first_name() + "\" not found\n";
        }
        if(!msg.empty()) throw std::runtime_error("Invalid set of arguments! Errors:\n"+msg);
    }

    std::string CLI::help() const
    {
        std::ostringstream strm;
        strm << _name << std::endl;
        return strm.str();
    }
}

#endif