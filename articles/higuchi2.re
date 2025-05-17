= Figma MCP Server x VS Code: Next.jsプロジェクトにFigmaデザインを適用する方法

//lead{
FigmaデザインをコードとしてWebサイトに実装するのは通常、デザイナーとエンジニアの連携が必要な工程ですが、Figmaの開発者向けツールとGitHub Copilotを組み合わせることで、AIがこのプロセスを大幅に自動化できるようになりました。この記事では、VS CodeからFigma MCP (Model Context Protocol) Serverに接続し、Next.jsプロジェクトに直接Figmaデザインを適用する方法を紹介します。

このアプローチを使えば、Figmaデザインを確認しながら、AIがそのデザインをHTMLやCSS、Reactコンポーネントとして実装してくれるため、開発時間を大幅に短縮できます。特にプロトタイプ作成や初期開発段階で威力を発揮するでしょう。
//}

== 事前準備

このガイドを進める前に、以下のものが必要です。

 * Node.js (v18以上)
 * VS Code最新版
 * GitHub Copilot
 * Figmaアカウントとアクセストークン

Figmaのアクセストークンは、Figmaの設定から取得できます。以下の手順で取得してください。

 1. Figmaにログイン
 2. プロフィールアイコンをクリックし、「Settings」を選択
 3. 「Personal Access Tokens」セクションで「Create a new token」をクリック
 4. トークンに名前を付けて生成

//image[setting][FigmaTop]{
//}

//image[setting2][FigmaSetting]{
//}

== 手順

=== Next.js + Shadcn + Tailwindでプロジェクトを作成

まずは基本となるNext.jsプロジェクトを作成します。ここではShadcn UIとTailwind CSSを使ったプロジェクトを構築します。

//cmd{
# プロジェクトの作成
npx create-next-app@latest figma-next-app --typescript

# プロジェクトディレクトリに移動
cd figma-next-app

# Tailwind CSSのセットアップ (Next.js 13以降は標準で含まれています)
# Shadcn UIをセットアップ
npx shadcn@latest init
//}

インタラクティブセットアップで質問されたら以下のように回答してください。

 * TypeScript：Yes
 * Style：Default (Tailwind CSS)
 * React Server Components：Yes
 * App Router：Yes
 * Import Alias：@/*（デフォルト）

これでShadcn UIコンポーネントをインストールできるようになりました。必要に応じて、以下のようにコンポーネントを追加できます。

//cmd{
npx shadcn@latest add button card dialog
//}

=== VS CodeでAgent Modeを有効化してMCP Server接続設定

次にVS Codeで作成したプロジェクトを開き、Figma MCP Serverと接続するための設定を行います。

 1. VS Codeを開き、画面左下歯車アイコンからSettingを選びます。
 2. MCPで検索
 3. Edit in setting.jsonをクリックして、出てきたjsonのmcpを以下のように変更する。

//source[setting.json]{
"mcp": {
  "inputs": [],
  "servers": {
    "figma-developer-mcp": {
      "command": "npx",
      "args": [
        "-y",
        "figma-developer-mcp",
        "--figma-api-key=自分のFigmaAPIキー",
        "--stdio"
      ]
    }
  }
}
//}

@<code>{自分のFigmaAPIキー}の部分は、先ほど取得したFigmaのアクセストークンに置き換えてください。

==== Figma MCP Serverのインストールと設定

Figma MCP Serverが利用できるように、まずはインストールします。

//cmd{
npm install -g figma-developer-mcp
//}

@<b>{注意}: VS Code設定を変更した後は、VS Codeを再起動するか、コマンドパレットから「Developer: Reload Window」を実行する必要があります。

=== Figma MCP Serverへの接続

次に、Figma MCP Serverを起動してVS Codeと接続します。以下の方法があります。

==== 方法1: ターミナルから手動で起動

ターミナルを開いて以下のコマンドを実行し、Figma MCP Serverを起動します。

//cmd{
npx figma-developer-mcp --figma-api-key=自分のFigmaAPIキー
//}

サーバーが起動したら、VS Codeのコマンドパレットを開き、「Chat: Agent Mode」を選択してサーバーに接続します。

==== 方法2: VS Code内からサーバーを起動して接続

VS Codeのコマンドパレットから直接「Chat: Agent Mode」を選択します。Agent Modeメニューが表示されたら「figma-developer-mcp」を選択します。先ほど設定したMCP設定に基づいてサーバーが自動的に起動します。

==== 接続確認

接続が成功すると、以下の現象が確認できます。

 * setting.jsonの対象行にRunningと表示される

接続に問題がある場合は、設定を確認し、以下を試してください。

 1. VS Codeを再起動する
 2. Figma APIキーが正しいか確認する
 3. ネットワーク接続を確認する
 4. ログレベルをdebugに設定して詳細情報を確認する

=== プロンプトからFigmaのデザイン実装を指示

==== 基本的な実装プロセス

VS Codeのチャットビューを開き、次のようなプロンプトでFigmaデザインの実装を依頼します。

//cmd{
Figmaから次のファイル情報を取得し、そのデザインをNext.jsとTailwind CSSを使って実装してください。

https://www.figma.com/design/FILE_KEY/FILE_NAME?node-id=NODE_ID&t=TOKEN
//}

FILE_KEY、FILE_NAME、NODE_ID、TOKENの部分はFigmaのデザインURLから取得できる値に置き換えてください。

Github Copilotはまず以下の内部ツールを使用してFigmaデータを取得します。

 * @<code>{get_figma_data} - Figmaデザイン全体の情報を取得
 * @<code>{download_figma_images} - 必要な画像をダウンロード

その後、以下のような処理を自動的に行います。

 1. Figmaファイルの解析と設計意図の理解
 2. 必要なコンポーネントとレイアウト構造の特定
 3. Next.js + Shadcn + Tailwindを使ったコード実装
 4. 共通コンポーネントの作成とスタイル適用
 5. デザインの実装

==== 効果的なプロンプトの例

より具体的な実装を依頼する場合は、以下のようなプロンプトも効果的です。

//cmd{
次のFigmaデザインをNext.jsとTailwind CSSを使って実装してください。
https://www.figma.com/design/FILE_KEY/FILE_NAME?node-id=NODE_ID

特に以下の点に注意してください：
1. コンポーネントを適切に分割し、再利用可能な設計にする
2. Tailwind CSSでスタイルを正確に再現する
3. レスポンシブデザインに対応する
4. アクセシビリティに配慮する
5. ダークモード対応も考慮する
//}

実装プロセスは規模によって数分から数十分かかることがあります。進行状況はチャットビューで確認できます。デザインが複雑な場合、GitHub CopilotはFigmaデータを分析しながら、追加の質問をすることもあります。

=== 表示確認

実装が完了したら、開発サーバーを起動して結果を確認しましょう。

//cmd{
npm run dev
//}

ブラウザで@<code>{http://localhost:3000}にアクセスすると、Figmaデザインが実装されたウェブページが表示されます。

実装に修正が必要な場合は、チャットを通じて具体的に指示することができます。

//cmd{
ナビゲーションバーの色を #f3f4f6 に変更してください。
//}

=== 変換事例

Figmaプロジェクト生成時にWebのdefaultとして表示されるテンプレートを今回の方法で変換した結果を以下に示します。
かなり差異があるため、実際のプロジェクトでは手動での調整が必要です。
しかし、ゼロベースで実装するより、これをベースとする事で時間短縮が期待できます。

//image[figma][Figmaデザイン]{
//}

//image[nextjs][Figmaデザインから生成したNext.js画面]{
//}

== 高度な使用方法

=== トラブルシューティング

Figma MCP Serverの使用中に問題が発生した場合、以下の点を確認してください。

==== 接続の問題

 * Figma APIキーが正しく設定されているか確認
 * ネットワーク接続に問題がないか確認
 * VS Codeのバージョンが最新か確認（遅延や互換性の問題を解決することがあります）

==== 実装の問題

 * Figmaデザインの構造が複雑すぎる場合は、より小さなコンポーネントに分割して実装を依頼
 * コンポーネント単位で実装を段階的に進める
 * 特定の部分にフォーカスした具体的なプロンプトを使用する

==== サーバーの問題

 * ログレベルをdebugに設定してエラー情報を確認
 * ターミナルから手動で起動して詳細なエラーメッセージを確認
 * 特定のFigmaファイルで問題が発生する場合は、そのファイルの共有設定と権限を確認

=== コマンドラインオプション一覧

Figma MCP Serverは以下のコマンドラインオプションをサポートしています。

//cmd{
オプション:
  --stdio                    VS Codeとの通信にSTDIOを使用 (必須)
  --figma-api-key=<キー>     Figma APIキー (必須)
  --port=<ポート>            使用するポート番号 (デフォルト: 9000)
  --log-level=<レベル>       ログレベル: debug, info, warn, error (デフォルト: info)
  --timeout=<ミリ秒>         リクエストタイムアウト時間 (デフォルト: 30000)
  --cache-dir=<パス>         キャッシュディレクトリのパス
  --no-cache                 キャッシュを無効化
  --help                     ヘルプを表示
//}

== まとめ

VS CodeからFigma MCP Serverに接続することで、FigmaデザインをNext.jsプロジェクトに直接適用する方法を紹介しました。この方法を使うことで、デザインからコードへの変換プロセスが大幅に効率化され、開発時間の短縮につながります。

特に注目すべき点は、次のものです。

 1. デザインとコードの連携がスムーズになり、デザイナーとエンジニア間のコミュニケーションコストが削減される
 2. 自動生成されたコードをベースに調整するだけで済むため、実装時間が短縮される
 3. コンポーネントベースのアプローチにより、再利用性と保守性が高まる
 4. フロントエンド開発の効率化だけでなく、デザイン品質の一貫性も確保できる
 5. ラピッドプロトタイピングや迅速なMVP開発に特に有効

今後のアップデートでさらに機能が拡充されることが期待されますが、現時点でもFigmaデザインの実装を大幅に効率化できるツールとして活用できるでしょう。

== よくある質問（FAQ）

//list[faq][よくある質問][text]{
Q: Figma MCP Serverとは何ですか？
A: Figma MCP（Model Context Protocol）Serverは、FigmaのデザインデータをAIモデルが理解しやすい形式で提供するサーバーです。これによりGitHub Copilotなどの生成AIがFigmaデザインを解析し、実装コードを生成できるようになります。

Q: どのようなFigmaデザインが実装しやすいですか？
A: コンポーネント化されていて、整理されたレイヤー名を持つデザインが最も実装しやすいです。また、一般的なUIパターンやコンポーネントを使用したデザインの方が、非常に独自性の高いカスタムデザインよりも正確に実装されやすい傾向があります。

Q: Figma MCP Serverを使うのに追加費用はかかりますか？
A: Figma MCP Server自体は無料で利用できますが、Figma APIへのアクセスには有効なFigmaアカウントが必要です。また、GitHub Copilotの使用には別途サブスクリプションが必要です。

Q: 実装されたコードは本番環境で使用できますか？
A: 生成されたコードは基本的な機能とスタイルを再現しますが、本番環境で使用する前には以下の点を確認することをお勧めします：
 1. アクセシビリティの確認
 2. パフォーマンスの最適化
 3. クロスブラウザ互換性のテスト
 4. セキュリティの確認

Q: 画像や特殊なフォントはどのように扱われますか？
A: Figmaデザイン内の画像はダウンロードされ、プロジェクトに含まれる想定ですが、現状では含まれていませんでした。今後の改良に期待しています。
//}
