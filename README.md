# Hexo 博客创建文档和发布步骤

## 简介

本项目是一个基于 Hexo 的静态博客站点，使用 Cactus 主题。博客托管在 GitHub Pages 上。

## 环境要求

- Node.js (推荐版本 14 或更高)
- npm 或 yarn
- Git

## 安装依赖

首先，克隆仓库并安装依赖：

```bash
git clone https://github.com/0rigina1/0rigina1.github.io.git
cd 0rigina1.github.io
npm install
```

## 创建新文章

使用 Hexo 命令创建新文章：

```bash
npx hexo new post "文章标题"
```

这将在 `source/_posts/` 目录下创建一个新的 Markdown 文件，文件名格式为 `文章标题.md`。

## 编辑文章

打开生成的 Markdown 文件，使用 Markdown 语法编写内容。文件位于 `source/_posts/` 目录。

文章头部需要包含 Front Matter，例如：

```yaml
---
title: 文章标题
date: 2025-03-05 12:00:00
tags:
- 标签1
- 标签2
categories: 分类
---
```

## 生成静态文件

生成静态文件到 `public/` 目录：

```bash
npm run build
# 或
npx hexo generate
```

## 本地预览

启动本地服务器预览博客：

```bash
npm run server
# 或
npx hexo server
```

访问 `http://localhost:4000` 查看预览。

## 发布到 GitHub Pages

1. 提交更改到 Git：

```bash
git add .
git commit -m "添加新文章：文章标题"
git push origin main
```

2. GitHub Actions 或手动部署会自动生成并部署静态文件到 GitHub Pages。

如果需要手动部署（如果没有配置自动部署）：

```bash
npx hexo clean
npx hexo generate
npx hexo deploy
```

注意：部署配置需要在 `_config.yml` 中设置。如果使用 GitHub Pages，需要配置 `deploy.type: git` 并设置仓库信息。

## 常用命令

- `npx hexo clean`: 清除缓存和生成的静态文件
- `npx hexo generate`: 生成静态文件
- `npx hexo server`: 启动本地服务器
- `npx hexo deploy`: 部署到配置的平台
- `npx hexo new page "页面名"`: 创建新页面
- `npx hexo new draft "草稿名"`: 创建草稿

## 主题配置

当前使用 Cactus 主题。主题配置文件位于 `themes/cactus/_config.yml`。

## 故障排除

- 如果遇到生成错误，尝试 `npx hexo clean` 后再生成
- 检查 Node.js 和 npm 版本
- 确保所有依赖已正确安装

## 更多信息

- [Hexo 官方文档](https://hexo.io/docs/)
- [Cactus 主题文档](https://github.com/probberechts/hexo-theme-cactus)
