//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef ISLAY_IMGUI_APPS_H
#define ISLAY_IMGUI_APPS_H

#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll;     // Keep scrolling if already at the bottom

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
            // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
            // Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
            // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};

static void DrawJsonConfig(std::string jsonName, const rapidjson::Document &config) {
    auto array2string = [](rapidjson::GenericValue<rapidjson::UTF8<>>::ConstArray array) -> std::string {

        std::string str;
        int row = array.Size();
        int col;
        if(array[0].IsArray()) {
            col = array[0].Size();
            str = "[ ";
            for (int j = 0; j < row; j++) {
                str += "[ ";
                for (int i = 0; i < col; i++) {
                    str = str + std::to_string(array[j][i].GetDouble()) + " ";
                }
                if(j == row-1){
                    str += "]";
                } else {
                    str += "], ";
                }
            }
            str += " ]";
        } else {
            str = "[ ";
            for (int i = 0; i < row; i++) {
                str = str + std::to_string(array[i].GetDouble()) + " ";
            }
            str += " ]";
        }

        return str;
    };

    ImGui::Begin(jsonName.c_str());
    for (rapidjson::Value::ConstMemberIterator itr = config.MemberBegin(); itr != config.MemberEnd(); itr++) {
        const rapidjson::Value &n = itr->name;
        const rapidjson::Value &v = itr->value;
        switch (v.GetType()) {
            case rapidjson::kNullType:       //!< null
                break;
            case rapidjson::kFalseType:      //!< false
                ImGui::Text("%s: false", n.GetString());
                break;
            case rapidjson::kTrueType :       //!< true
                ImGui::Text("%s: true", n.GetString());
                break;
            case rapidjson::kObjectType :    //!< object
                break;
            case rapidjson::kArrayType :     //!< array
                ImGui::Text("%s: %s", n.GetString(), array2string(v.GetArray()).c_str());
                break;
            case rapidjson::kStringType :    //!< string
                ImGui::Text("%s: %s", n.GetString(), v.GetString());
                break;
            case rapidjson::kNumberType :    //!< number
                if (v.IsInt()) {
                    ImGui::Text("%s: %d", n.GetString(), v.GetInt());
                } else if (v.IsDouble()) {
                    ImGui::Text("%s: %lf", n.GetString(), v.GetDouble());
                } else if (v.IsFloat()) {
                    ImGui::Text("%s: %f", n.GetString(), v.GetFloat());
                }
                break;
        }
    }
    ImGui::End();
}

#endif //ISLAY_IMGUI_APPS_H
