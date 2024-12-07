## 概述

本项目是北京邮电大学网络工程专业2024年编译原理语法分析实验的C++实现，使用xmake构建，支持Windows和Linux平台。

## 安装

### Windows

1. **安装 xmake:**

   从 [xmake 发布页面](https://github.com/xmake-io/xmake/releases) 下载适用于 Windows 的最新 xmake 安装程序。

   运行安装程序并按照说明完成安装。

2. **验证安装:**

   打开命令提示符并运行：

   ```sh
   xmake --version
   ```

   你应该会看到已安装的 xmake 版本。

### Linux

1. **安装 xmake:**

   你可以使用以下命令安装 xmake：

   ```sh
   curl -fsSL https://xmake.io/shget.text | bash
   ```

2. **验证安装:**

   打开终端并运行：

   ```sh
   source ~/.xmake/profile
   xmake --version
   ```

   你应该会看到已安装的 xmake 版本。

## 构建项目

1. **克隆仓库:**

   ```sh
   git clone https://github.com/xxxhois/LL1Parser.git
   cd LL1Parser
   ```

2. **构建项目:**

   运行以下命令来构建项目：

   ```sh
   xmake -v
   ```

   这将根据 `xmake.lua` 文件中的配置编译项目。

## 运行项目

构建项目后，你可以使用以下命令运行它：

```sh
xmake run
```