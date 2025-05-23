= 2025年のAIについて考える

はじめまして、あるいはお久しぶりです、@<tt>{@ken11}です。2025年に古巣であるグリーグループのグリーエックス株式会社に帰ってきました。

さて、ChatGPTが世に出てから2年以上が経過し、AI/LLMを取り巻く環境はそれを専門に取り扱う自分から見ても大きく変わりました。ここでは2025年におけるAIを取り巻く世界の事情や市場感などに対して筆者が考えていることを中心にお話しします。

== 主要AIモデルの進化・変遷

主要なAIモデルの市場は大きく変化してきました。その中心となっているのは次の3つのモデルです。

 * ChatGPT（OpenAI）
 * Claude（Anthropic）
 * Gemini（Google）

これらに加えて、Amazonは興味深い動きを見せています。彼らはClaudeを開発するAnthropicに巨額の出資@<fn>{amazon-anthropic}を行う一方で、自社でもTitanの後継となるAmazon Novaをリリース@<fn>{nova}し、AIモデル市場での存在感を高めています。

//footnote[amazon-anthropic][https://www.nikkei.com/article/DGXZQOGN22DND0S4A121C2000000/]
//footnote[nova][https://aws.amazon.com/jp/ai/generative-ai/nova/]


特筆すべきは、ChatGPTの進化の方向性です。リリース当初から「AGI（汎用人工知能）」到達への期待を背負い、より大きなパラメータサイズ、より多くの学習データを使って進化を続けてきました。その流れは技術的な進歩として一定の成果を上げています。

しかし、2024年頃からその流れに変化が見られるようになりました。OpenAIはAGIへの道筋について語る一方で、SF映画『her／世界でひとつの彼女』を彷彿とさせる音声会話体験を大々的にアピール@<fn>{her}するなど、別の路線にも注力し始めています。

//footnote[her][SF映画のような自然な会話体験を実現したOpenAIの音声チャットデモ：https://www.youtube.com/watch?v=vgYi3Wr7v_g]

一方でGeminiとNovaは自社インフラを最大限に活用した低コスト戦略を強めています。現在では、他のモデルのおよそ1/10のコストで提供されているケースもあり、コスト効率を重視する企業にとっては注目の存在です。

== 変わる世界

変わったのはAIモデル自体だけではありません。この「なんとなく便利そうなもの」をどうにかこうにかビジネスにつなげようと、数多のスタートアップが登場し、さまざまな新サービスが市場に投入されています。

もはや世界はAIに対して「付加価値」ではなく「当然のもの」として位置づけはじめていて、@<b>{「AIオリエンテッド」な時代}が訪れようとしています。AIを機能として取り入れることを前提としたサービス設計が求められ、同時に、AIを使うことで発生する問題に対して初期段階からよく考えておく必要が生じています。

特に注目すべき変化として、次の2点が挙げられます。

=== AIネイティブの台頭

ChatGPTがリリースされた時点で10代だった世代が、AIを当たり前のツールとして使いこなす「AIネイティブ」として社会に出始めています。彼らの働き方やコミュニケーション方法は従来の世代と大きく異なり、企業文化にも変化をもたらしています。

彼らはAIを使わない世界を知らず、問題解決の第一歩としてAIに相談することを当然と考えています。@<fn>{seo}この世代がさらに社会に浸透していくことで、AIとの協働を前提とした働き方が一般化していくでしょう。

//footnote[seo][これはAIネイティブ世代に限った話ではなく、AIの台頭による検索数への影響については近頃よく語られています：https://www.nikkei.com/article/DGXZQOGM24BHC0U5A420C2000000/]

=== 「AI疲れ」と「適材適所」への回帰

初期の万能感から一転して、AIの限界も明らかになってきました。幻覚（ハルシネーション）問題は依然として解決されておらず、重要な意思決定や創造的な業務では人間の判断が不可欠であるという認識が広まっています。

「AIにすべて任せればよい」という考え方から、「AIと人間の適材適所」を模索する段階に入りつつあります。特に正確性が求められる医療や法律分野、創造性が求められるクリエイティブ分野では、AIは人間の判断を補助する存在として位置づけられるようになっています。

このように、AIを取り巻く環境は技術的進化と社会的受容の両面で大きく変化しています。次節では、このような状況下で独自AIモデルを開発する意義について考えていきます。

== 独自モデルをつくる意味

初期のAIブーム時には、（当時の）ChatGPT相当のパラメータサイズのモデルを独自に開発しようという動きがBERTの延長として盛んに語られていました。しかし、2025年の現在、その意義は大きく変わっています。ここでは独自モデル開発の現実と意味について考えていきます。

=== 大手AIと独自モデルのコストの考え方

大手のAIサービスは各社が「nトークンあたりいくら」というコスト体系を採用しています。たとえば、Claude 3.5はAmazon Bedrockを使った東京リージョンで1000トークンの入力で0.003ドル、出力で0.015ドル、バッチ処理ならそれぞれその半額となっています。@<fn>{bedrock-cost}

//footnote[bedrock-cost][https://aws.amazon.com/jp/bedrock/pricing/]

この料金体系の最大の利点は「使った分しかかからない」という点です。利用量に応じて費用が発生するため、使用量が少ない場合のコスト効率がとても高くなります。

一方で、独自モデルを常時ホストするとなると固定費として月に数万〜数百万円がかかってくることになります。たとえば、Llama 2の7Bパラメータモデルがギリギリ動作するAWS Inferentiaの@<tt>{Inf2.xlarge}のインスタンスは、1時間あたり1.1373ドルの費用がかかります@<fn>{inf2-cost}。

//footnote[inf2-cost][https://aws.amazon.com/jp/ec2/pricing/on-demand/]

この1時間あたり1.1373ドルというのは、先ほどのBedrock Claude換算で約6万トークンの入力と6万トークンの出力に相当します。
つまり、それだけのリクエスト量が見込めないと、BedrockでClaudeを使った場合のコストより低く抑えることができないということになります。

これは毎秒16トークンの入力と出力がある状態と考えると、かなりの利用量が必要です。しかも、これは比較的小さな7Bパラメータモデルの場合です。NovaやGeminiといったコスト効率の高いモデルと比較すると、もはや7Bモデルでもコスト面では大手AIサービスにかなわない状況になっています。

現実にコスト面で独自モデルが活きてくるのは、バッチ処理で十分なタスク（常時ホストする必要がないタスク）などです。特化した内容に対して少ないトークン数で確実に結果を出すような場面でこそ、真価を発揮すると筆者は考えます。

=== 実用的な代替策

現在では、独自モデルを一から開発するよりも、既存の大規模モデルをベースにして自社のニーズに合わせる手法が主流になっています。

特に注目されているのが次のアプローチです：

 1. ファインチューニング：既存の大規模モデルを特定のタスクや領域向けに調整する手法
 2. RAG（検索拡張生成）：外部知識ベースを参照して回答の精度を高める手法
 3. プロンプトエンジニアリング：適切な指示を与えることでモデルの出力を制御する手法

これらの方法は、独自モデルをゼロから開発するよりも低コストで高い効果を得られることが多く、2025年の現在では標準的なアプローチとなっています。

=== 本当に必要なケース

コストを度外視した場合でも、独自モデルが本当に必要になるケースは限定的です。主に次のような場面で意義を持ちます：

 * 高度な専門性を要する領域（医療・法律など）で、独自のデータを使って学習やファインチューニングすることに強い意味や優位性をもたらす場合
 * セキュリティやプライバシーの観点から、データを外部サービスに送信できない場合
 * 特殊な言語や業界用語など、一般的なモデルでは対応できない特殊なドメイン知識が必要な場合

しかし、こういった場面でも、すべてのケースで独自モデルが必要というわけではありません。大手AIが提供している超巨大モデルに対してRAGなどの仕組みを組み合わせることで、より効率的に高品質なアウトプットを得られるシーンも多いでしょう。

結局のところ、独自モデル開発の判断はケースバイケースといわざるを得ませんが、ここで伝えたいのは次の3点です：

 1. 大手AIサービスのコストが大幅に下がり、リアルタイム性も向上したことで実用性が飛躍的に高まっている
 2. そのような状況下で「独自モデルを作る」という選択肢の意義は相対的に薄くなってきている
 3. 独自のデータがあったとしても、それを単純に学習させて独自モデルを作るよりも、大手のAIサービスとうまく連携させる（プロンプト設計やRAGなどを含めて）方が、多くの場合でコスト効率が高い

2025年の現在、AIの活用においては@<b>{「作る」よりも「使いこなす」技術がより重要}になっているのです。

== MCPとAIA、言葉の一人歩き

2025年になって途端に「MCP」と「AIA」という言葉を頻繁に耳にするようになりました。業界誌の見出しやカンファレンスのセッションタイトル、そして新規プロジェクトの提案書にもこれらの用語が踊っています。しかし、これらの言葉は本当に革新的な概念なのでしょうか？

=== MCPとは？

MCP（Model Context Protocol）は、Anthropicが2024年11月に発表したオープンスタンダードで、AIアシスタントと外部データソースやツールを接続するための標準プロトコルです@<fn>{mcp-def}。これにより、AIモデルは訓練データの枠を超えて、リアルタイムの情報にアクセスしたり、外部ツールを利用したりできます。

//footnote[mcp-def][2025年初頭から急速に人気を集め、多くの開発者やサービスが採用するようになった。 https://www.anthropic.com/news/model-context-protocol]

MCPの基本アーキテクチャはクライアント-サーバーモデルに基づいており、AIアプリケーション（クライアント）が外部のデータソースやツール（サーバー）と標準化された方法で通信できるようにします。「AIアプリケーションのためのUSB-Cポート」とも表現される技術です@<fn>{mcp-usb}。

//footnote[mcp-usb][USB-Cがデバイスと周辺機器を接続する標準化された方法を提供するように、MCPはAIモデルと異なるデータソースやツールを接続する標準化された方法を提供する。 https://docs.anthropic.com/ja/docs/agents-and-tools/mcp]

2025年初頭には、GitHubやGoogle Drive、Slackなど多くの人気サービスとの接続を可能にするMCPサーバーが登場し、エコシステムが急速に拡大しています。しかし、この「革新的」と喧伝されるプロトコルは、実際には既存の技術を標準化しただけという側面も否めません。

MCPがもたらすのはよりシームレスなAIと外部サービスの連携体験であり、「MCPを使えば解決する」というようなことはなく、どちらかというと今までは個々人の開発者が頑張っていた部分を統一的なプロトコルで解決してくれるようになった、と捉えるべきものです。

=== AIAとは？

AIA（AI Agent）は、自律的に判断して目的を達成できるAIシステムを指します@<fn>{aia-def}。単純な質問応答だけでなく、複数ステップのタスクを自律的に処理し、状況や環境に応じて柔軟に行動を変化させるのが特徴です。いうなれば、人間の指示を待たずに自ら考えて動くアシスタントのような存在です。

//footnote[aia-def][2025年は「AIエージェント元年」とも呼ばれ、国内でも「AI Agent Day 2025」など専門のカンファレンスが開催されている。]

2024年から2025年にかけて、AIAはビジネス領域での導入が加速し、特にコンタクトセンター、マーケティング、営業支援などの分野で活用が進んでいます。

しかし、このAIAという概念も、実際にはSiriやAlexaの延長線上にあるものです。OpenAIは先日AIA機能「Operator」をリリースしましたが、これは人間に代わって旅行予約などをしてくれるものです。@<fn>{operator}
「AIに家の電気をつけさせる」「AIに天気を教えさせる」といったユースケースは、音声アシスタントの登場時点ですでに構想されていたものであり、「AIA」と呼称されるようになっただけです。

こういったAIないしはそれ以前のオートメーション技術の使い方は約20年前のPlaggerの頃から変わっていません。

//footnote[operator][人類はなぜか、コンピュータにピザを注文させたがる傾向があるようです。]

=== 本質を見極める視点

実際のところ、これら2つのキーワードが2025年のトレンドとして騒がれるということは、何を意味しているのでしょうか？それは冒頭で述べたとおり、AIが実用段階に入り、「AIオリエンテッド」な世界になってきたことを示しているに過ぎません。

MCPのような標準プロトコルやAIAのような自律型AIシステムが注目されるのは、AIの活用が本格化し、実用的なシステムへの統合が求められるようになったからこそです。言い換えれば、これらのバズワードは、AIが「おもちゃ」から「道具」へと変わりつつあることの証左なのです。

これらのキーワードや技術を押さえていれば十分というわけではありません。むしろ、これらの概念がなぜ注目されているのかという背景を理解し、自分のビジネスや業務にどう活かせるかを考えることが重要です。

また、見落としてはならないのは、これらのキーワードはAIそのものの技術的進歩を表すものではないということです。AIの基礎技術（モデルアーキテクチャや学習アルゴリズムなど）の進化とは別の、応用面でのトレンドに過ぎません。

バズワードに惑わされず、その背後にある本質的な変化—AIが日常的なツールとなり、あらゆるシステムに組み込まれていく流れ—を理解することが、2025年のAI時代を生き抜くための鍵となるでしょう。

そして私たちエンジニアに求められるのは、これらの言葉の流行に一喜一憂するのではなく、AIを実際の問題解決にどう活かすかという実践的な視点です。次節では、そうした観点からエンジニアにとってのAIの意味について考えていきます。

== エンジニアリングにおける2025年AIの旅

エンジニアにとっても普段の開発シーンにAIを活用する場面が増えてきています。先行していたGitHub Copilotのみではなく、DeepSeek Coder、Cursor、Devin、OpenHandsなど、開発生産性を高めるためのツールがどんどんと登場しています。これらのツールは、コーディングの方法だけでなく、エンジニアの役割そのものにも変化をもたらしつつあります。

=== 変わるエンジニアの役割

筆者はよくOpenHandsとavante.nvimを使って開発していますが、その結果コードよりも日本語を書いている時間の方が多くなりました。

これは筆者が個人的に技術書典に出している本でも書いたことですが、OpenHandsなどによりよいアウトプットをさせるためには、詳細でわかりやすいIssueづくりが欠かせません。それはチーム開発で新人エンジニアに対しても丁寧なIssueを渡すこととなんら変わらないことであり、詳細に仕様を書き下し、細部まで実装方針を決めてあげる必要があるということです。そのために多くの時間を日本語を書くことに費やす結果となっています。

また、AIが生成したアウトプットをしっかりとレビューすることも必要です。AIはどこからみつけてきたのか不明な、セキュリティホールしかないような実装をたまに提供してきます@<fn>{ai-security}。レビューもAIにやらせることはできますが、それでも結局最後には自分の目で検査する必要があるのです。

//footnote[ai-security][特に外部パッケージの安易な使用や古いAPIの実装など、セキュリティリスクを含むコードを生成することがしばしばある。]

そう、エンジニアの役割はAIの登場でより上流工程に移っていっているのです。仕様を考え、設計を行い、生成されたコードをレビューするという、より「考える」部分に注力するようになっています。

=== 開発現場の実態

とはいえ、実際に手を動かすのはAIが全部やって完結するかというと、まだまだそんなことはありません。極端な話、実態としてはエンジニアが今までGoogle検索してドキュメントや実装例を見ながら考えて書いていたものが、AIと対話して書いていくようになっただけくらいのものです。

偉い人は「AI活用により開発生産性を遙かに高める」と言いますが、現実はそんなに甘くはありません。前述のとおりAIは誤った実装を出してくることがあるますし、そもそもAIに的確に指示を出すというのはより多くの日本語を書かないといけなくて面倒なことが多いのです。

AIが開発生産性を遙かに高める日はまだ遠いかもしれません。しかし、だからといって諦めてしまえばそんな日は永遠に来ないでしょう。現在のAIツールを上手に使いこなすことで、少しずつでも生産性を向上させていくことが大切です。

=== 実践的活用法

それでは具体的に、エンジニアはAIツールをどのように活用すべきでしょうか？ いくつかの実践的なアプローチを紹介します。

==== 効果的なプロンプト設計

AIによい結果を出させるためには、明確で具体的な指示が不可欠です。筆者の経験では、次のような要素を含めるとよりよい結果が得られます：

 1. コンテキストの明確化：どんなプロジェクトで、どんな目的でコードを書いているのかを説明する
 2. 要件の詳細化：機能要件、非機能要件、エッジケースなどを具体的に記述する
 3. 制約条件の明示：使用すべき技術スタック、避けるべきアプローチなどを伝える
 4. スタイルガイドの参照：コーディング規約があれば、それに従うよう指示する

これらの情報をAIに与えることで、より適切なコードが生成される確率が高まります。

==== レビューのポイント

AIが生成したコードをレビューする際は、次の点に特に注意するとよいでしょう：

 1. セキュリティの脆弱性：特に入力検証、認証、暗号化などの部分
 2. パフォーマンス：不必要なループや非効率なアルゴリズムがないか
 3. エラーハンドリング：例外処理が適切に実装されているか
 4. テスト可能性：単体テストが書きやすい構造になっているか
 5. ドキュメント：コメントや説明が適切に含まれているか

これらの点をチェックすることで、AIが生成したコードの質を向上させることができます。

いずれにせよ、AIはまだ完璧なツールではありません。しかし、その限界を理解した上で上手に活用することで、エンジニアの仕事の質と効率を高めることができるでしょう。

== AI待ちの時代から未来へ

最近、「AI待ち」をしている人が増えているように思えてなりません。ChatGPTに質問して結果が出るのを待ったり、Deep Researchを依頼して結果が届くのを待ったり、AIがコードを書き終わるのを待ったり。こうした「AI待ち」の時間が人類にとってだんだんと増えているのではないでしょうか。

筆者は、特にコードについては今のところAIが書くのを待っているくらいなら自分で書けばいいと考えています。結局その方が早いことも多いですし、手戻りもなくてよいのです。

「AIがやってくれるから」という理由だけでAIを使っていると、よくわからない待ち時間を持て余し、ショート動画を眺めて時間を無為に過ごす「考えない葦」ができあがってしまいます。「AIを使うべきシーン」とそうでないシーンはよく考えて、@<b>{ただ便利だからという理由だけで使うことのないように}したいものです。

=== AGIは（まだ）こない

ここまで2025年のAI動向を話してきましたが、筆者はAGI（汎用人工知能）についてはまだ来ないと考えています。

現状のAIはコスト（電力コストと計算時間コスト）の問題を抱えています。これはあの日人類に「Attention is All You Need」@<fn>{transformer}がもたらした「Transformer」が莫大な積和演算を必要としているからです。

//footnote[transformer][2017年にGoogleの研究者らが発表した論文のタイトル。Transformerアーキテクチャを提案し、現在の大規模言語モデルの基礎となった。 https://arxiv.org/abs/1706.03762]

いま主流のAIはいずれもこのTransformerをベースにしており、積和演算のためのコストがとても高くなっています。AGIにたどり着くには、Transformerの次のアーキテクチャが出現するか、あるいはTransformerの計算を奇跡的な低コストで実現できるハードウェアが出てこないと無理だろうと考えています。

現に冒頭で紹介したように、OpenAIですらSF映画『her／世界でひとつの彼女』を思わせる音声会話体験を大々的にアピールするなど、AGIとは別の路線に言及し始めています。彼らも単純な知能の進化についてはそろそろ限界を感じているのではないかと筆者は考えています。これまでパラメータサイズを大きくすることで性能を向上させてきた手法が、いよいよ頭打ちになりつつあるのかもしれません。

だから「AGIが出てくるからやっても無駄」なんてことはまだ当分ないと思います。

=== いつかの未来に備えて

とはいえ、AGIがいつかは来るでしょうし、現在のAIも十分にすごいですから、便利に使わないわけにはいきません。

繰り返しになりますが、使いどころをしっかり見極めて使うことが重要です。たとえば：

 * 大量のデータから特定のパターンをみつけ出す作業
 * 繰り返し性の高い定型的なコーディング
 * クリエイティブな発想のきっかけが欲しいとき
 * 外国語のコミュニケーション支援

こうした場面では積極的にAIを活用することで、生産性を大きく向上させることができるでしょう。

一方で、次のような場面ではAIに頼りすぎないほうがよいかもしれません：

 * 重要な意思決定を行うとき
 * 高度なセキュリティを要する実装
 * 創造的な表現を追求するとき
 * 人間の感情や文脈を深く理解する必要があるとき

要するに、AIを使うか使わないかという二元論ではなく、「どのように」「どのシーンで」使うかを考えることが大切なのです。

そしていつかは訪れるであろうその日に向けて、しっかりとAIオリエンテッドな世界に順応し、AIの当たり前を享受しましょう。しかしその一方で、人間としての思考力、創造性、判断力は決して手放さないでください。それこそが私たち人間のもつ、かけがえのない価値なのですから。
