= VertexAI Agent Engine x Flutter: デジタルマーケティングAIエージェントの作成

TODO: この構成でオモチャをいくつか作り、技術書典と相性の良いものを記事にする。とりあえず、試しに作ったもので記事を書いておく。

== はじめに
Cloud Next 25で発表されたVertexAI Agent Engineを使用することで、自然言語処理を活用したエージェントを簡単に構築できます。
これを利用して、デジタルマーケティングに役立つAIエージェントを構築します。
フロントエンドにはFlutterを利用し、マルチデバイスに対応します。
本記事では、AIエージェントの作成からFlutterアプリケーションの実装までを解説します。

== AIエージェントの概要
デジタルマーケティングを検討している企業に対して、AIエージェントがサポートを行います。
AIエージェントは、デジタルマーケティングの専門家として振る舞います。
AIエージェントは、自社の商品マスターデータベースを参照し、適切な商品を提案します。

AIエージェントの開発からデプロイまで、Jupyter Notebookを使用して行います。
テキスト情報を受け取り、json形式で返すAPIを作成します。
このAPIは、Flutterアプリケーションから呼び出されます。

TODO: ここにシステム構成図を入れる

=== Notebookの準備
Vertex AI WorkbenchのNotebookを使用します。以下の手順でセットアップしてください。

//footnote[fuga][Colabやanacondaでも動作しますが、認証が必要です。説明を簡素化するため、Vertex AI Workbenchを使用します。]

TODO: ここにセットアップ手順を入れる

=== Notebook実装
Notebookに以下の実装を行なって聞きます。

==== 環境構築
まず、以下のライブラリをインストールします。

//listnum[pip-install][bash]{
%pip install --upgrade --quiet \
    "google-cloud-aiplatform[agent_engines,langchain]" \
    cloudpickle==3.0.0 \
    "pydantic>=2.10" \
    requests
//}

インストールしたライブラリを有効化するため、Jupyter Notebookのカーネルを再起動します。

//listnum[shutdown][python]{
import IPython

app = IPython.Application.instance()
app.kernel.do_shutdown(True)
//}

次にVertex AI SDKを初期化します。

//listnum[init][python]{
PROJECT_ID = "[your-project-id]"  # @param {type:"string"}
LOCATION = "us-central1"  # @param {type:"string"}
STAGING_BUCKET = "gs://[your-staging-bucket]"  # @param {type:"string"}

import vertexai

vertexai.init(project=PROJECT_ID, location=LOCATION, staging_bucket=STAGING_BUCKET)
//}

次にライブラリをインポートします。

//listnum[import][python]{
from vertexai import agent_engines
from vertexai.preview.reasoning_engines import LangchainAgent
model = "gemini-2.0-flash"
//}

==== 商品マスターデータベースへのアクセス
次に、エージェントが使用する商品マスターへのアクセスを実装します。

TODO: とりま作ったのは配列から取得するものだった。Spannerから取得するものに変更して書く。

==== AIエージェントの作成
エージェントは、LangchainAgentを使用して作成します。

TODO: プロンプトチューニングでデジタルマーケティングの専門家として振る舞うようにする。

//listnum[create-agent][python]{
agent = LangchainAgent(
    model=model,
    tools=[get_exchange_rate],
    agent_executor_kwargs={"return_intermediate_steps": True},
)
//}

==== デプロイ
作成したエージェントをデプロイします。

//listnum[deploy-agent][python]{
remote_agent = agent_engines.create(
    agent,
    requirements=[
        "google-cloud-aiplatform[agent_engines,langchain]",
        "cloudpickle==3.0.0",
        "pydantic>=2.10",
        "requests",
    ],
)
//}

==== APIの確認
エージェントを呼び出してみます。

//listnum[query-agent][python]{
from vertexai import agent_engines

AGENT_ENGINE_RESOURCE_NAME = remote_agent.resource_name
print(AGENT_ENGINE_RESOURCE_NAME)
query = "デジタルマーケティングのトレンドは？"

remote_agent = agent_engines.get(AGENT_ENGINE_RESOURCE_NAME)
response = remote_agent.query(input=query)
print(response)
//}

これでAIエージェントの作成が完了しました。
次に、フロントエンドとなるFlutterアプリケーションを作成します。

== Flutterアプリの作成
TODO: Flutterアプリの作成手順を記載する。生成AIに実装させ、その手順を書いた方が面白いかも。

== まとめ
SageMakerを使えば、以前から同様のことができましたが、Vertex AI Agent Engineを使うことで、より簡単にエージェントを構築できるようになりました。
