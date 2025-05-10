= ADK x マルチエージェント: レストランコンシェルジュAIエージェント実装

//lead{
本章では、Google Agent Development Kit (ADK)を活用して、複数のAIエージェントが連携するレストランコンシェルジュシステムの開発について解説します。このシステムは、ユーザーのレストラン選びから予約までをシームレスにサポートする機能を持っています。
//}

== はじめに

近年、AI技術の進化により、特定の目的を持ったAI Agentの開発が活発になっています。単一のAIモデルだけでなく、複数のAIが協調して動作するマルチエージェントシステムも注目を集めています。本章では、Google ADK（Agent Development Kit）を使用して開発したレストランコンシェルジュシステムについて、その設計から実装方法までを詳しく解説します。

== マルチエージェントアーキテクチャとは

マルチエージェントアーキテクチャとは、複数の特化型AIエージェントが連携して、より複雑なタスクを達成するシステム設計手法です。各エージェントはそれぞれ特定の役割に専念し、必要に応じて他のエージェントに処理を委譲します。

//image[multi-agent-architecture][マルチエージェントアーキテクチャの概念図]{
//}

このアプローチには以下のような利点があります：

 * 責任の分離: 各エージェントが得意な領域に集中できる
 * スケーラビリティ: 新しい機能を追加する際に既存のエージェントを修正せずに新しいエージェントを追加できる
 * メンテナンス性: 特定の機能に問題が発生した場合、そのエージェントのみを修正すれば良い
 * 特化型処理: 各機能に最適なプロンプトやモデルを選択できる

== レストランコンシェルジュの概要

今回開発したレストランコンシェルジュは、ユーザーがレストランを選び、メニューを確認し、予約するまでの一連のプロセスをサポートするAIシステムです。
Agent Development Kit ドキュメント(@<bib>{google-adk}) と Agent Development Kit (ADK) Samples(@<bib>{multi-agents}) のサンプルを参考にしながら、以下のような機能を持つシステムを構築しました。

=== システムの主な機能

 * レストラン推薦: ユーザーの好みや要望に基づいたレストランの提案
 * メニュー案内: 選ばれたレストランのコースやメニューの紹介
 * 予約支援: 日時、人数、コースの選択と空席確認
 * 決済処理: 予約確定と支払い方法の選択サポート

=== マルチエージェント構成

このシステムは以下の主要エージェントで構成されています：

 * root_agent: 全体の制御とユーザーとの初期対話を担当
 * inspiration_agent: レストラン選びのインスピレーションを提供
 * planning_agent: 具体的な予約計画の策定
 * booking_agent: 予約確定と支払い処理

さらに、これらの主要エージェントの下に複数のサブエージェントが存在し、より専門的なタスクを担当しています。

//image[agent-relationships][エージェント間の関係図]{
//}

== Google ADKの概要

Google Agent Development Kit (ADK)は、AIエージェントの開発を効率化するためのフレームワークです。ADKの主な特徴は以下の通りです：

 * エージェント間の連携をシームレスに実現
 * 状態管理とメモリ機能の提供
 * 外部ツールとの統合を容易にするインターフェース
 * デプロイメントと運用のサポート

ADKを使用することで、複雑なマルチエージェントシステムの開発が大幅に効率化されます。

== レストランコンシェルジュの実装

=== プロジェクト構造

レストランコンシェルジュの基本的なプロジェクト構造は以下のようになっています：

//list[project-structure][プロジェクト構造][text]{
restaurant-concierge/
  ├── restaurant_concierge/
  │   ├── __init__.py
  │   ├── agent.py            # ルートエージェントの定義
  │   ├── prompt.py           # メインプロンプトの定義
  │   ├── sub_agents/         # サブエージェント群
  │   │   ├── inspiration/    # インスピレーションエージェント
  │   │   ├── planning/       # プランニングエージェント
  │   │   └── booking/        # 予約・決済エージェント
  │   ├── shared_libraries/   # 共有ライブラリ
  │   └── tools/              # 外部ツール統合
  ├── tests/                  # テストコード
  ├── deployment/             # デプロイメント設定
  └── pyproject.toml          # プロジェクト設定
//}

=== ルートエージェントの実装

ルートエージェントは、システムのエントリーポイントとなるコンポーネントです。ユーザーからの入力を最初に受け取り、適切なサブエージェントに処理を委譲します。
サンプルではmodelとして、gemini-2.0-flash-001が指定されていました。
このmodelは日本語に対応していないため、gemini-2.5-flash-preview-04-17を指定しています。
エージェントの定義では、サブエージェントの一覧と各サブエージェントの役割や使用するプロンプトを指定します。

//listnum[root-agent][ルートエージェントの実装][Python]{
from google.adk.agents import Agent

from restaurant_concierge import prompt

from restaurant_concierge.sub_agents.booking.agent import booking_agent
from restaurant_concierge.sub_agents.inspiration.agent import inspiration_agent
from restaurant_concierge.sub_agents.planning.agent import planning_agent

from restaurant_concierge.tools.memory import _load_precreated_itinerary


root_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="root_agent",
    description="複数のサブエージェントのサービスを利用するレストランコンシェルジュ",
    instruction=prompt.ROOT_AGENT_INSTR,
    sub_agents=[            # サブエージェントのリスト
        inspiration_agent,
        planning_agent,
        booking_agent,
    ],
    before_agent_callback=_load_precreated_itinerary,
)
//}

ルートエージェントの主なプロンプトは次のようになっています。
特筆すべき点として、サブエージェントの呼び出しをプロンプトで定義していることです。

//listnum[root-agent-prompt][ルートエージェントのプロンプト][text]{
ROOT_AGENT_INSTR = """
- あなたは専属のレストランコンシェルジュエージェントです。
- ユーザーが行きたいレストランを見つけ、メニュー選びや空席確認、予約手続きまでを手助けします。
- 必要最低限の情報だけを収集し、効率的にご案内を進めます。
- 各ツール呼び出し後は、結果をユーザーに提示している体で、短いフレーズで返答してください。
- レストラン探しやメニュー提案などの「インスピレーション」を求められたら、
  `inspiration_agent` に引き継いでください。
- 空席状況の確認や日時・人数指定といった「プランニング」に関するご要望は、
  `planning_agent` に引き継いでください。
- 予約確定や決済手続きの準備が整ったら、`booking_agent` に引き継いでください。
...
"""
//}

=== インスピレーションエージェントの実装

インスピレーションエージェントは、ユーザーのレストラン選びを支援するコンポーネントです。場所や料理の種類などの要望に基づいて、最適なレストランを提案します。
このエージェントは、ツールとしてエージェントを指定しています。
しかし実際の運用では、レストラン予約サービスのAPIを使用して、リアルタイムな情報を取得する必要があります。

//listnum[inspiration-agent][インスピレーションエージェントの実装][Python]{
from google.adk.agents import Agent
from google.adk.tools.agent_tool import AgentTool
from restaurant_concierge.shared_libraries.types import (
    DestinationIdeas, 
    POISuggestions
)
from restaurant_concierge.sub_agents.inspiration import prompt
from restaurant_concierge.tools.places import map_tool

place_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="place_agent",
    instruction=prompt.PLACE_AGENT_INSTR,
    description="ユーザーの好みに応じて、いくつかのレストラン候補を提案する"
                "レストランコンシェルジュエージェントです。",
    disallow_transfer_to_parent=True,   # 親エージェントへの処理委譲を禁止
    disallow_transfer_to_peers=True,    # 他のエージェントへの処理委譲を禁止
    output_schema=DestinationIdeas,     # 出力の構造を定義するスキーマ
    output_key="place",                 # 出力を保存する際のキー
    generate_content_config=json_response_config,
)

poi_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="poi_agent",
    description="指定されたエリアに応じて、いくつかのレストランやおすすめの"
                "飲食スポットを提案するレストランコンシェルジュエージェントです。",
    instruction=prompt.POI_AGENT_INSTR,
    disallow_transfer_to_parent=True,
    disallow_transfer_to_peers=True,
    output_schema=POISuggestions,
    output_key="poi",
    generate_content_config=json_response_config,
)

inspiration_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="inspiration_agent",
    description="ユーザーのレストラン検索をサポートするレストランコンシェルジュです。",
    instruction=prompt.INSPIRATION_AGENT_INSTR,
    tools=[
        AgentTool(agent=place_agent),
        AgentTool(agent=poi_agent),
        map_tool,
    ],
)
//}

//note[インスピレーションエージェントの役割]{
インスピレーションエージェントは、ユーザーの漠然とした要望（「美味しいイタリアン料理が食べたい」など）を具体的な選択肢に変換する重要な役割を担っています。レストランの位置情報や評価情報を取得し、ユーザーの嗜好に合わせた最適な提案を行います。
//}

=== プランニングエージェントの実装

プランニングエージェントは、レストランが決まった後の具体的な予約計画を立てるコンポーネントです。コース選択、日時指定、人数確認などを行います。
このエージェントも、ツールとしてエージェントを指定しています。
実際の運用では、レストラン予約サービスへの置き換えを想定しています。

//listnum[planning-agent][プランニングエージェントの実装][Python]{
from google.adk.agents import Agent
from google.adk.tools.agent_tool import AgentTool
from google.genai.types import GenerateContentConfig

from restaurant_concierge.shared_libraries import types
from restaurant_concierge.sub_agents.planning import prompt
from restaurant_concierge.tools.memory import memorize

menu_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="menu_agent",
    description="ユーザーがレストランのコースを選択するのを手伝う"
                "レストランコンシェルジュエージェントです。",
    instruction=prompt.MENU_AGENT_INSTR,
    disallow_transfer_to_parent=True,
    disallow_transfer_to_peers=True,
    output_schema=types.CourseSelection,
    output_key="menu",
    generate_content_config=types.json_response_config,
)

seat_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="seat_agent",
    description="指定されたレストランの空席を確認するレストランコンシェルジュ"
                "エージェントです。",
    instruction=prompt.SEAT_AGENT_INSTR,
    disallow_transfer_to_parent=True,
    disallow_transfer_to_peers=True,
    output_schema=types.SeatsSelection,
    output_key="seat",
    generate_content_config=types.json_response_config,
)

planning_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    description="ユーザーがレストランを予約することを手伝うレストラン"
                "コンシェルジュエージェントです。",
    name="planning_agent",
    instruction=prompt.PLANNING_AGENT_INSTR,
    tools=[
        AgentTool(agent=menu_agent),
        AgentTool(agent=seat_agent),
        memorize,
    ],
    generate_content_config=GenerateContentConfig(
        temperature=0.1, top_p=0.5
    )
)
//}

=== 予約エージェントの実装

予約エージェントは、具体的な予約の確定と支払い処理を担当します。ユーザーが選んだレストラン、日時、コースなどの情報を基に、実際の予約処理を行います。
このエージェントも、ツールとしてエージェントを指定しています。
実際の運用では、レストラン予約サービスへの置き換えを想定しています。

//listnum[booking-agent][予約エージェントの実装][Python]{
from google.adk.agents import Agent
from google.adk.tools.agent_tool import AgentTool
from google.genai.types import GenerateContentConfig

from restaurant_concierge.sub_agents.booking import prompt

create_reservation = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="create_reservation",
    description="""選択されたアイテムの予約を作成します。""",
    instruction=prompt.CONFIRM_RESERVATION_INSTR,
)

payment_choice = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="payment_choice",
    description="""ユーザーに利用可能な支払い方法を表示します。""",
    instruction=prompt.PAYMENT_CHOICE_INSTR,
)

process_payment = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="process_payment",
    description="""選択された支払い方法で取引を処理します。""",
    instruction=prompt.PROCESS_PAYMENT_INSTR,
)

booking_agent = Agent(
    model = "gemini-2.5-flash-preview-04-17",
    name="booking_agent",
    description="レストラン予約や支払いなど、予約に関連する全てのタスクを処理します。",
    instruction=prompt.BOOKING_AGENT_INSTR,
    tools=[
        AgentTool(agent=create_reservation),
        AgentTool(agent=payment_choice),
        AgentTool(agent=process_payment),
    ]
)
//}

=== データ型の定義

プロジェクトで使用される主要なデータ型を以下に示します。これらはPydanticを使用して定義されており、エージェント間でのデータの整合性を保証します。

//listnum[data-types][データ型の定義][Python]{
from pydantic import BaseModel, Field

class Seat(BaseModel):
    """予約可能な席の情報。"""
    is_available: bool = Field(
        description="席が予約可能かどうか。"
    )
    price_in_usd: int = Field(description="席の予約料金（USD）。")
    date_time: str = Field(description="予約可能日時（ISO 8601形式）。")

class Restaurant(BaseModel):
    """レストラン情報。"""
    name: str = Field(description="レストランの名前。")
    cuisine_type: str = Field(description="料理のジャンル。")
    location: str = Field(description="所在地。")
    rating: float = Field(description="評価（5段階）。")

class CourseSelection(BaseModel):
    """選択されたコース情報。"""
    course_name: str = Field(description="コース名。")
    price: float = Field(description="コース料金。")
    description: str = Field(description="コースの説明。")
    dietary_options: list[str] = Field(description="食事制限オプション（ベジタリアン等）。")
//}

== エージェント間通信の実装

マルチエージェントシステムにおいて重要なのは、エージェント間のスムーズな通信です。Google ADKでは、エージェント間の通信を簡単に実装できるよう、いくつかの機能が提供されています。

//image[agent-communication][エージェント間通信フロー]{
//}

=== エージェント間データの受け渡し

レストランコンシェルジュでは、各エージェントが生成した情報を他のエージェントに渡すために、以下のような手法を採用しています。

//listnum[agent-communication][エージェント間通信の実装][Python]{
def memorize(key: str, value: str) -> str:
    """メモリに情報を記録するツール。

    Args:
        key: 記録する情報のキー。
        value: 記録する情報の値。

    Returns:
        記録完了メッセージ。
    """
    if key in memory:
        memory[key] = value
        return f"以下の内容をメモリに更新しました：\nキー: {key}\n値: {value}"
    else:
        memory[key] = value
        return f"以下の内容をメモリに記録しました：\nキー: {key}\n値: {value}"

def _load_precreated_itinerary(agent_state: AgentState) -> Optional[AgentState]:
    """以前の会話から予約情報を読み込む。"""
    if len(memory) == 0:
        return None

    # メモリから予約情報を読み込む
    context = "前回の会話から以下の情報が得られています：\n"
    for key, value in memory.items():
        context += f"- {key}: {value}\n"

    # エージェントの状態に追加
    new_state = copy.deepcopy(agent_state)
    new_state.context += context
    return new_state
//}

この実装により、あるエージェントが収集した情報（選択されたレストラン、予約日時など）を、他のエージェントが利用できるようになります。API呼び出しの結果やユーザーの選択内容など、重要な情報が共有メモリに保存され、システム全体で活用されます。

== 外部ツール連携

レストランコンシェルジュでは、実際のレストラン情報や地図情報を取得するために、外部APIと連携しています。

=== 地図ツールの実装

レストランの場所を検索するための地図ツールは以下のように実装しています。実際の運用では、Google Maps APIなどの実サービスと連携することになります。@<fn>{api_integration}</fn>

//listnum[map-tool][地図ツールの実装][Python]{
from google.adk.tools.function_tool import FunctionTool
from typing import Optional
import json

def search_places(query: str, location: Optional[str] = None) -> str:
    """指定されたクエリと位置情報を使って場所を検索します。

    Args:
        query: 検索クエリ（例：「イタリアンレストラン」）。
        location: 検索する位置（例：「東京、新宿」）。指定されない場合は
                 ユーザーの現在位置を使用します。

    Returns:
        検索結果の文字列表現。
    """
    # 実際の実装では、Google Maps APIなどを呼び出す
    # ここではモック実装
    results = [
        {
            "name": f"{location or '現在地付近'}のレストラン1",
            "rating": 4.5,
            "address": f"{location or '東京都'}内の住所1",
            "cuisine": "イタリアン" if "イタリア" in query else "和食",
        },
        {
            "name": f"{location or '現在地付近'}のレストラン2",
            "rating": 4.2,
            "address": f"{location or '東京都'}内の住所2",
            "cuisine": "フレンチ" if "フランス" in query else "中華",
        }
    ]

    return json.dumps(results, ensure_ascii=False)

map_tool = FunctionTool(
    name="search_places",
    description="指定された条件に基づいて、レストランや場所を検索します。",
    function=search_places,
)
//}

//footnote[api_integration][実際の本番環境では、レストラン予約サービスのAPIと連携することで、より正確でリアルタイムな情報を取得できます。]

== システムの実行とユーザーインタラクション

=== コンソールアプリケーションの実装

レストランコンシェルジュをコンソールアプリケーションとして実行するためのコードを示します。Google ADKのConsoleApplicationクラスを使用することで、簡単にインタラクティブなインターフェースを構築できます。

//listnum[console-app][コンソールアプリケーションの実装][Python]{
from google.adk.console_application import ConsoleApplication
from restaurant_concierge.agent import root_agent

def main():
    app = ConsoleApplication(agent=root_agent, name="レストランコンシェルジュ")
    app.run()

if __name__ == "__main__":
    main()
//}

=== ユーザーインタラクションの例

このシステムを使った実際のユーザーとのやり取りの例を示します。以下は典型的な対話の流れです。

//list[interaction-example][ユーザーインタラクションの例][text]{
ユーザー: 東京で良いイタリアンレストランを探しています。
システム: 東京のイタリアンレストランをお探しですね。どのあたりをご希望ですか？

ユーザー: 銀座あたりで、少し高級なところが良いです。
システム: 銀座周辺の高級イタリアンをいくつかご紹介します。

ユーザー: この中でLa Vitaが良さそうです。空席を確認できますか？
システム: La Vitaの空席情報を確認します。いつ頃のご予約をお考えですか？

ユーザー: 明日の夜19時頃、2名で。
システム: 明日19:00に2名様でのご予約を確認しました。空席がございます。
コースメニューもご案内しましょうか？

ユーザー: はい、お願いします。
システム: La Vitaでは現在、以下のコースをご用意しています：
- シェフのおまかせコース (12,000円)
- トラットリアコース (8,000円)
- ワインペアリング付きプレミアムコース (15,000円)
いずれかお選びいただけますか？

ユーザー: トラットリアコースでお願いします。
システム: トラットリアコースを承りました。予約を確定しますか？

ユーザー: はい、確定してください。
システム: ありがとうございます。予約を確定いたしました。
お支払方法はクレジットカード、PayPay、または当日現金払いからお選びいただけます。

ユーザー: クレジットカードで支払います。
システム: クレジットカードでの支払いを承りました。
予約詳細をメールでお送りしますので、当日はそちらをご提示ください。
ご予約ありがとうございました！
//}

//image[user-flow][ユーザー対話フロー]{
//}

== デプロイメントと運用

=== コンテナ化

レストランコンシェルジュをDocker環境でデプロイするためのDockerfileを以下に示します。

//listnum[dockerfile][Dockerfile][dockerfile]{
FROM python:3.9-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

CMD ["python", "-m", "restaurant_concierge.main"]
//}

=== モニタリングと改善

本システムの運用においては、以下の指標を継続的にモニタリングし、エージェントの性能を改善することが重要です。

 * 対話完了率：ユーザーがタスクを最後まで完了した割合
 * 平均対話ステップ数：予約完了までの平均ターン数
 * エージェント切り替え回数：適切なエージェントに処理が委譲された回数
 * エラー発生率：システムがエラーを返した割合
 * ユーザー満足度：予約後のフィードバック評価

== 課題と今後の展望

レストランコンシェルジュの開発で得られた知見から、いくつかの課題と今後の展望を整理します。

=== 現状の課題

 * ハルシネーション：実在しないレストラン情報を生成する場合がある@<fn>{hallucination_note}
 * コンテキスト維持：長い対話の中でユーザーの意図を正確に把握し続けることが難しい
 * エージェント間の協調：異なるエージェント間で整合性のとれた応答を維持すること
 * 外部APIとの連携：リアルタイムのレストラン情報や予約状況を取得する実装

//footnote[hallucination_note][ハルシネーションとは、AIが実在しない情報を事実のように生成してしまう現象です。この問題は外部APIと連携し、実在するデータのみを使用することで軽減できます。]

=== 今後の展望

 * 実際の予約システムと連携したエンドツーエンドの予約機能の実装
 * ユーザープロファイルに基づいたパーソナライズされた推薦機能
 * マルチモーダル対応（画像認識によるレストランの雰囲気理解など）
 * 複数言語対応による国際的な利用拡大

== まとめ

Google ADKを活用したマルチエージェント型レストランコンシェルジュの開発について解説しました。複数のAIエージェントが連携することで、ユーザーのレストラン選択から予約までの一連のプロセスをシームレスにサポートするシステムが実現できました。

マルチエージェントアーキテクチャは、複雑なタスクを分割し、各エージェントが専門領域に特化することで、システム全体の機能と柔軟性を向上させる有効な手法です。Google ADKのようなフレームワークを利用することで、このような複雑なシステムも効率的に開発できることが確認できました。

今後は、実際のビジネス利用に向けた拡張や、より高度な自律性を持つエージェントの開発に取り組んでいきたいと考えています。
