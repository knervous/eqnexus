export module nk;
import <functional>;
import <string>;
import "nuklear_d3d9.h";

export namespace nk_util
{
void
DrawSeparator(struct nk_context* ctx)
{
    float height = ctx->current->layout->row.height;
    nk_layout_row_dynamic(ctx, 1, 1);
    nk_spacing(ctx, 1);
    ctx->current->layout->row.height = height;
}

void
DrawBlockWithBorder(struct nk_context* ctx)
{
    struct nk_color border_color = nk_rgb(198, 170, 132);
    float border_thickness       = 2.0f;
    nk_layout_row_dynamic(ctx, 1, 1);
    struct nk_rect rect = nk_rect(0, 0, 100, 2);
    nk_stroke_rect(&ctx->current->buffer, rect, 0, border_thickness, border_color);
}

void
RenderDisabledButton(struct nk_context* ctx, const std::string& text)
{
    struct nk_style_button original_button_style = ctx->style.button;

    struct nk_style_button disabled_button_style = original_button_style;
    disabled_button_style.normal                 = nk_style_item_color(nk_rgb(40, 40, 40));
    disabled_button_style.hover                  = nk_style_item_color(nk_rgb(40, 40, 40));
    disabled_button_style.active                 = nk_style_item_color(nk_rgb(40, 40, 40));
    disabled_button_style.border_color           = nk_rgb(60, 60, 60);
    disabled_button_style.text_background        = nk_rgb(60, 60, 60);
    disabled_button_style.text_normal            = nk_rgb(160, 160, 160);
    disabled_button_style.text_hover             = nk_rgb(160, 160, 160);
    disabled_button_style.text_active            = nk_rgb(160, 160, 160);

    ctx->style.button = disabled_button_style;

    nk_button_label(ctx, text.c_str());

    ctx->style.button = original_button_style;
}

std::string
TruncateTextWithEllipsis(struct nk_context* ctx, const char* text, float max_width)
{
    const char* ellipsis = "...";
    float ellipsis_width = ctx->style.font->width(ctx->style.font->userdata, ctx->style.font->height, ellipsis, 3);

    int len          = strlen(text);
    float text_width = ctx->style.font->width(ctx->style.font->userdata, ctx->style.font->height, text, len);

    if (text_width <= max_width)
    {
        return text;
    }
    std::string truncated_text;
    for (int i = 0; i < len; ++i)
    {
        truncated_text += text[i];
        float current_width =
            ctx->style.font->width(ctx->style.font->userdata, ctx->style.font->height, truncated_text.c_str(), truncated_text.size());
        if (current_width + ellipsis_width > max_width)
        {
            truncated_text = truncated_text.substr(0, truncated_text.size() - 1) + ellipsis;
            break;
        }
    }
    return truncated_text;
}

}  // namespace nk_util
