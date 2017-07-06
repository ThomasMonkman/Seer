import { Injectable } from '@angular/core';
import { CookieService } from 'angular2-cookie/services/cookies.service';
import { Observable } from 'rxjs/Observable';
import { ReplaySubject } from 'rxjs/ReplaySubject';

@Injectable()
export class ActiveConnectionsService {
  private readonly cookieKey: string = 'activeConnections';
  private connections: string[];
  private readonly connectionsSubject: ReplaySubject<string[]> = new ReplaySubject(1);
  constructor(private cookieService: CookieService) {
    const connectionCookie = this.cookieService.get(this.cookieKey);
    this.connections = connectionCookie.length > 0 ? JSON.parse(connectionCookie) : [];
    this.connectionsSubject.next(this.connections);
  }

  public get(): Observable<string[]> {
    return this.connectionsSubject.share();
  }

  public store(connection: string) {
    this.connections.push(connection);
    this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }

  public remove(connection: string) {
    this.connections = this.connections.filter(_connection => _connection === connection);
    this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }
}
