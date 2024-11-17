export module server;

import <string>;
import <functional>;
import <vector>;
import <tuple>;

using set_context_t = std::function<void(std::string)>;

export class Server
{
   public:
    static void RegisterCallback(const std::string& id, set_context_t cb)
    {
        ContextCallbacks.push_back({id, cb});
    }
    static void RemoveCallback(const std::string& remove_id)
    {
        ContextCallbacks.erase(std::remove_if(ContextCallbacks.begin(),
                                              ContextCallbacks.end(),
                                              [&remove_id](const auto& tuple) { return std::get<0>(tuple) == remove_id; }),
                               ContextCallbacks.end());
    }
    static void SetContext(const std::string& context)
    {
        Context = context;
        for (const auto& [id, cb] : ContextCallbacks)
        {
            cb(Context);
        }
    }
    static void Init()
    {
        Context          = "";
        ContextCallbacks = {};
    }
    static void Teardown()
    {
        // Fire one last set of callbacks - other hooks depend on change listeners for their own
        // init/teardown
        SetContext("");
        ContextCallbacks.clear();
    }
    static const std::string& GetContext()
    {
        return Context;
    }

   private:
    inline static std::string Context                                                  = "";
    inline static std::vector<std::tuple<std::string, set_context_t>> ContextCallbacks = {};
};
